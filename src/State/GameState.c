/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "GameState.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../Moves/Move.h"
#include "../Moves/MoveCollection.h"
#include "../Moves/MoveGeneration.h"

struct state {
    uint64_t bitboards[2][6];       /* [color][piece] */
    bool     castling_rights[4];    /* indexed by castle_t */
    uint64_t en_passant_target_square;
    color_t  to_move_color;
    int      half_move_count;
    int      full_move_count;
};

state_t *new_state(void) {
    state_t *state = calloc(1, sizeof(state_t));
    state->to_move_color = WHITE;
    return state;
}

void free_state(state_t *state) {
    free(state);
}

state_t *clone_state(const state_t *src) {
    state_t *dst = malloc(sizeof(state_t));
    memcpy(dst, src, sizeof(state_t));
    return dst;
}

// FEN loader

void load_fen_string(state_t *state, const char *fen) {
    memset(state->bitboards, 0, sizeof(state->bitboards));
    memset(state->castling_rights, 0, sizeof(state->castling_rights));
    state->en_passant_target_square = 0;
    state->half_move_count = 0;
    state->full_move_count = 1;

    const char *p = fen;

    // piece placement, rank 8 first
    int rank = 7, file = 0;
    while (*p && *p != ' ') {
        if (*p == '/') {
            rank--;
            file = 0;
        } else if (*p >= '1' && *p <= '8') {
            file += *p - '0';
        } else {
            color_t color = isupper((unsigned char)*p) ? WHITE : BLACK;
            piece_t piece;
            switch (tolower((unsigned char)*p)) {
                case 'p': piece = PIECE_PAWN;   break;
                case 'r': piece = PIECE_ROOK;   break;
                case 'n': piece = PIECE_KNIGHT; break;
                case 'b': piece = PIECE_BISHOP; break;
                case 'q': piece = PIECE_QUEEN;  break;
                case 'k': piece = PIECE_KING;   break;
                default:  piece = NULL_PIECE;   break;
            }
            if (piece != NULL_PIECE)
                state->bitboards[color][piece] |= 1ULL << (rank * 8 + file);
            file++;
        }
        p++;
    }
    if (*p == ' ') p++;

    // active color
    state->to_move_color = (*p == 'b') ? BLACK : WHITE;
    p++;
    if (*p == ' ') p++;

    // castling rights
    while (*p && *p != ' ') {
        switch (*p) {
            case 'K': state->castling_rights[CASTLE_KINGSIDE_WHITE]  = true; break;
            case 'Q': state->castling_rights[CASTLE_QUEENSIDE_WHITE] = true; break;
            case 'k': state->castling_rights[CASTLE_KINGSIDE_BLACK]  = true; break;
            case 'q': state->castling_rights[CASTLE_QUEENSIDE_BLACK] = true; break;
        }
        p++;
    }
    if (*p == ' ') p++;

    // en passant
    if (*p != '-') {
        int ep_file = *p - 'a'; p++;
        int ep_rank = *p - '1';
        state->en_passant_target_square = 1ULL << (ep_rank * 8 + ep_file);
    }
    while (*p && *p != ' ') p++;
    if (*p == ' ') p++;

    // half-move clock
    state->half_move_count = atoi(p);
    while (*p && *p != ' ') p++;
    if (*p == ' ') p++;

    // full-move number
    state->full_move_count = atoi(p);
}

// move application

static void handle_castling_placement(state_t *state, castle_t castle) {
    switch (castle) {
        case CASTLE_KINGSIDE_WHITE:
            // King e1->g1, Rook h1->f1
            state->bitboards[WHITE][PIECE_KING]  = 0x0000000000000040ULL;
            state->bitboards[WHITE][PIECE_ROOK] &= ~0x0000000000000080ULL;
            state->bitboards[WHITE][PIECE_ROOK] |=  0x0000000000000020ULL;
            break;
        case CASTLE_QUEENSIDE_WHITE:
            // King e1->c1, Rook a1->d1
            state->bitboards[WHITE][PIECE_KING]  = 0x0000000000000004ULL;
            state->bitboards[WHITE][PIECE_ROOK] &= ~0x0000000000000001ULL;
            state->bitboards[WHITE][PIECE_ROOK] |=  0x0000000000000008ULL;
            break;
        case CASTLE_KINGSIDE_BLACK:
            // King e8->g8, Rook h8->f8
            state->bitboards[BLACK][PIECE_KING]  = 0x4000000000000000ULL;
            state->bitboards[BLACK][PIECE_ROOK] &= ~0x8000000000000000ULL;
            state->bitboards[BLACK][PIECE_ROOK] |=  0x2000000000000000ULL;
            break;
        case CASTLE_QUEENSIDE_BLACK:
            // King e8->c8, Rook a8->d8
            state->bitboards[BLACK][PIECE_KING]  = 0x0400000000000000ULL;
            state->bitboards[BLACK][PIECE_ROOK] &= ~0x0100000000000000ULL;
            state->bitboards[BLACK][PIECE_ROOK] |=  0x0800000000000000ULL;
            break;
        default:
            break;
    }
}

static void handle_promotion(state_t *state, piece_t promo, uint64_t to_sq, color_t color) {
    if (promo == NULL_PIECE) return;
    state->bitboards[color][PIECE_PAWN] &= ~to_sq;
    state->bitboards[color][promo] |= to_sq;
}

static void revoke_castling_rights(state_t *state, const move_t *move) {
    color_t c = state->to_move_color;
    int offset = (c == WHITE) ? 0 : 2;
    const flags_t *f = get_move_flags(move);
    if (f->king_moved) {
        state->castling_rights[CASTLE_KINGSIDE_WHITE  + offset] = false;
        state->castling_rights[CASTLE_QUEENSIDE_WHITE + offset] = false;
    }
    if (f->kingside_rook_moved)
        state->castling_rights[CASTLE_KINGSIDE_WHITE  + offset] = false;
    if (f->queenside_rook_moved)
        state->castling_rights[CASTLE_QUEENSIDE_WHITE + offset] = false;
}

void play_move(state_t *state, const move_t *move) {
    color_t mover = state->to_move_color;
    color_t opponent = (mover == WHITE) ? BLACK : WHITE;

    uint64_t from_sq = get_move_from_square(move);
    uint64_t to_sq = get_move_to_square(move);
    const flags_t *flags = get_move_flags(move);

    piece_t from_piece = get_piece_on_square(state, from_sq);
    piece_t to_piece = get_piece_on_square(state, to_sq);

    // remove any opponent piece on the destination square
    if (to_piece != NULL_PIECE)
        state->bitboards[opponent][to_piece] &= ~to_sq;

    // move our piece
    state->bitboards[mover][from_piece] &= ~from_sq;
    state->bitboards[mover][from_piece] |= to_sq;

    // en passant: captured pawn is one rank behind the destination
    if (from_piece == PIECE_PAWN
            && state->en_passant_target_square != 0
            && to_sq == state->en_passant_target_square) {
        uint64_t captured = (mover == WHITE) ? (to_sq >> 8) : (to_sq << 8);
        state->bitboards[opponent][PIECE_PAWN] &= ~captured;
    }

    revoke_castling_rights(state, move);

    state->en_passant_target_square = flags->en_passant_square;

    handle_promotion(state, flags->promotion_piece, to_sq, mover);

    if (flags->castle != NULL_CASTLE)
        handle_castling_placement(state, flags->castle);

    if (mover == BLACK) state->full_move_count++;

    state->to_move_color = opponent;
}

// queries

color_t side_to_move(const state_t *state) {
    return state->to_move_color;
}

void set_side_to_move(state_t *state, color_t color) {
    state->to_move_color = color;
}

piece_t get_piece_on_square(const state_t *state, uint64_t square) {
    for (int c = 0; c < 2; c++)
        for (int p = 0; p < 6; p++)
            if (state->bitboards[c][p] & square) return (piece_t)p;
    return NULL_PIECE;
}

color_t get_color_of_piece_on_square(const state_t *state, uint64_t square) {
    for (int c = 0; c < 2; c++)
        for (int p = 0; p < 6; p++)
            if (state->bitboards[c][p] & square) return (color_t)c;
    return NULL_COLOR;
}

uint64_t color_bb(const state_t *state, color_t color) {
    uint64_t bb = 0;
    for (int p = 0; p < 6; p++) bb |= state->bitboards[color][p];
    return bb;
}

uint64_t piece_bb(const state_t *state, piece_t piece, color_t color) {
    return state->bitboards[color][piece];
}

uint64_t get_en_passant_target(const state_t *state) {
    return state->en_passant_target_square;
}

bool is_en_passant_target_active(const state_t *state) {
    return state->en_passant_target_square != 0;
}

// castling legality -- path clearance only, check is handled in KingMoveGeneration

bool state_can_castle(const state_t *state, castle_t castle) {
    if (castle == NULL_CASTLE) return false;
    if (!state->castling_rights[castle]) return false;

    uint64_t occupied = color_bb(state, WHITE) | color_bb(state, BLACK);

    switch (castle) {
        case CASTLE_KINGSIDE_WHITE:  return (occupied & 0x0000000000000060ULL) == 0; // f1, g1
        case CASTLE_QUEENSIDE_WHITE: return (occupied & 0x000000000000000EULL) == 0; // b1, c1, d1
        case CASTLE_KINGSIDE_BLACK:  return (occupied & 0x6000000000000000ULL) == 0; // f8, g8
        case CASTLE_QUEENSIDE_BLACK: return (occupied & 0x0E00000000000000ULL) == 0; // b8, c8, d8
        default: return false;
    }
}

// check / checkmate / stalemate

// returns a stack copy of state with to_move_color set to color
// used to query attacks/moves from a specific side without allocating
static state_t as_color(const state_t *state, color_t color) {
    state_t tmp;
    memcpy(&tmp, state, sizeof(state_t));
    tmp.to_move_color = color;
    return tmp;
}

bool is_check(const state_t *state, color_t color) {
    uint64_t king_bb = piece_bb(state, PIECE_KING, color);
    if (king_bb == 0) return false;

    state_t tmp      = as_color(state, color);
    uint64_t attacked = get_attacked_squares_bitboard(&tmp);
    return (attacked & king_bb) != 0;
}

static bool has_no_legal_moves(const state_t *state, color_t color) {
    state_t tmp = as_color(state, color);
    move_collection_t *moves = get_legal_moves_of_state(&tmp);
    bool empty = move_collection_is_empty(moves);
    free_move_collection(moves);
    return empty;
}

bool is_checkmate(const state_t *state, color_t color) {
    return is_check(state, color) && has_no_legal_moves(state, color);
}

bool is_stalemate(const state_t *state, color_t color) {
    return !is_check(state, color) && has_no_legal_moves(state, color);
}

game_status_t get_state_status(const state_t *state) {
    if (is_checkmate(state, WHITE)) return WHITE_CHECKMATE;
    if (is_checkmate(state, BLACK)) return BLACK_CHECKMATE;
    if (is_stalemate(state, WHITE) || is_stalemate(state, BLACK)) return STALEMATE;
    if (is_check(state, WHITE)) return WHITE_CHECK;
    if (is_check(state, BLACK)) return BLACK_CHECK;
    return IN_GAME;
}
