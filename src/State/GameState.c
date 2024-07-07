#include "GameState.h"

#include <string.h>
#include <stdlib.h>

#include "../Moves/MoveCollection.h"
#include "../Moves/MoveGeneration.h"

/**
 * @struct state
 * @brief Represents the state of a chess game.
 *
 * This structure contains all the information needed to represent
 * the current state of a chess game.
 */
struct state {
    /**
     * @brief A 2D array representing the bitboards for each color and piece type.
     *
     * The first dimension is for the color (one to one mapped with color_t),
     * and the second dimension is for the piece type (one to one mapped with peice_t).
     */
    uint64_t bitboards[2][6];

    /**
     * @brief An array representing the castling rights for each color and side.
     *
     * The indices are mapped one to one with castle_t.
     */
    bool castling_rights[4];

    /**
     * @brief The en passant target square, represented as a bitboard.
     *
     * If there is no en passant target square, this value is 0.
     */
    uint64_t en_passant_target_square;

    color_t to_move_color;
    game_status_t status; 

    bool kingside_castle_is_possible;
    bool queenside_castle_is_possible;

    int half_move_count;
    int full_move_count;
};


/*
+=============================================================================+
|             State Creation & Destruction & Copying                          |
+=============================================================================+
*/

state_t *new_state() {
    state_t *state = (state_t *)malloc(sizeof(state_t));
    
    memset(state->bitboards, 0, sizeof(state->bitboards));    
    memset(state->castling_rights, 0, sizeof(state->castling_rights));

    state -> en_passant_target_square = 0;
    state -> kingside_castle_is_possible = false;
    state -> queenside_castle_is_possible = false;

    state->to_move_color = WHITE;
    state->status = UNDEFINED;
    return state;
}


void free_state(state_t *state) {
    free(state);
}


void copy_state(const state_t *fromState, state_t *toState) {
    memcpy(toState->bitboards, fromState->bitboards, sizeof(toState->bitboards));
}


/*
+=============================================================================+
|             Playing a Move                                                  |
+=============================================================================+
*/

void handle_castling_flags(state_t *state, const castle_t castle) {
    if (castle == CASTLE_KINGSIDE_BLACK) {
        state->bitboards[BLACK][PIECE_KING]  =  0x1000000000000000;
        state->bitboards[BLACK][PIECE_ROOK] |=  0x8000000000000000;
        state->bitboards[BLACK][PIECE_ROOK] &= ~0x1000000000000000;  
    } 
    
    else if (castle == CASTLE_QUEENSIDE_BLACK) {
        state->bitboards[BLACK][PIECE_KING]  =  0x1000000000000000;
        state->bitboards[BLACK][PIECE_ROOK] |=  0x1000000000000000;
        state->bitboards[BLACK][PIECE_ROOK] &= ~0x8000000000000000;
    } 
    
    else if (castle == CASTLE_KINGSIDE_WHITE) {
        state->bitboards[WHITE][PIECE_KING]  =  0x0000000000000010;
        state->bitboards[WHITE][PIECE_ROOK] |=  0x0000000000000001;
        state->bitboards[WHITE][PIECE_ROOK] &= ~0x0000000000000010;
    } 
    
    else if (castle == CASTLE_QUEENSIDE_WHITE) {
        state->bitboards[WHITE][PIECE_KING]  =  0x0000000000000010;
        state->bitboards[WHITE][PIECE_ROOK] |=  0x0000000000000010;
        state->bitboards[WHITE][PIECE_ROOK] &= ~0x0000000000000001;
    }
}


void handle_promotion_flag(state_t *state, const piece_t promotion_piece, uint64_t to_square) {
    if (promotion_piece == NULL_PIECE) return;
    state->bitboards[state->to_move_color][promotion_piece] |= to_square;
    state->bitboards[state->to_move_color][PIECE_PAWN] &= ~to_square;
}


void handle_castling_nullification_flags(state_t *state, const flags_t *flags) {
    int color_offset = (state->to_move_color == WHITE) ? 0 : 2;

    if (flags->king_moved) {
        state->castling_rights[CASTLE_KINGSIDE_WHITE + color_offset] = false;
        state->castling_rights[CASTLE_QUEENSIDE_WHITE + color_offset] = false;
    }

    if (flags->kingside_rook_moved) state->castling_rights[CASTLE_KINGSIDE_WHITE + color_offset] = false;
    if (flags->queenside_rook_moved) state->castling_rights[CASTLE_QUEENSIDE_WHITE + color_offset] = false;
}


void process_move_flags(state_t *state, const move_t *move) {
    const flags_t *flags = get_move_flags(move);

    // handle moves which update the en passant target square
    state->en_passant_target_square = flags->en_passant_square;

    handle_promotion_flag(state, flags->promotion_piece, get_move_to_square(move));
    handle_castling_flags(state, flags->castle);
    handle_castling_nullification_flags(state, flags);
}


void play_move(state_t *state, const move_t *move) {
    color_t to_move_c = state->to_move_color;
    color_t opponent_c = to_move_c == WHITE ? BLACK : WHITE;

    piece_t to_piece = piece_on_square(state, get_move_to_square(move));
    piece_t from_piece = piece_on_square(state, get_move_from_square(move));

    // remove any captured piece (if one exists)
    if (from_piece != NULL_PIECE) state->bitboards[opponent_c][to_piece] &= ~get_move_to_square(move);

    // move the from piece to the to square
    state->bitboards[to_move_c][from_piece] &= ~get_move_from_square(move);
    state->bitboards[to_move_c][from_piece] |= get_move_to_square(move);

    if (state->to_move_color == BLACK) state->full_move_count++;

    process_move_flags(state, move);
    state->to_move_color = opponent_c;
}


/*
+=============================================================================+
|            Status calculation                                               |
+=============================================================================+
*/

typedef struct {
    const state_t *state;
    const color_t to_move_color;
    const color_t opponent_color;
    const uint64_t king_square;
} king_status_t;



bool are_non_sliding_attackers(king_status_t status) {
    const uint64_t opp_knight_bitboard = status.state->bitboards[status.opponent_color][PIECE_KNIGHT];

    // Knight move offsets (in terms of squares)

    int pawn_offsets[] = status.to_move_color == WHITE ? (int[]){7, 9} : (int[]){-7, -9};
    int knight_offsets[] = {-17, -15, -10, -6, 6, 10, 15, 17};
    int king_offsets[] = {-9, -8, -7, -1, 1, 7, 8, 9};

    int num_pawn_offsets = sizeof(pawn_offsets)     / sizeof(pawn_offsets[0]);
    int num_knight_offsets = sizeof(knight_offsets) / sizeof(knight_offsets[0]);
    int num_king_offsets = sizeof(king_offsets)     / sizeof(king_offsets[0]);

    for (int i = 0; i < num_pawn_offsets; i++) {
        int offset = pawn_offsets[i];
        uint64_t offset_sq = offset > 0 ? status.king_square << offset : status.king_square >> -offset;

        // Check if the move is within the board and doesn't wrap around
        if (offset_sq < 64 && abs((offset_sq % 8) - (status.king_square % 8)) <= 2) {
            if (opp_knight_bitboard & (1ULL << offset_sq)) return true;
        }
    }

    return false;
}


bool in_check(const state_t *state) {
    color_t opponent_color = state->to_move_color == WHITE ? BLACK : WHITE;

    king_status_t status = {
        .state = state,
        .to_move_color = state->to_move_color,
        .opponent_color = opponent_color,
        .king_square = states_piece_bitboard(state, PIECE_KING, state->to_move_color)
    };

    if (!are_attacking_knight(status)) return false;
}



/*
+=============================================================================+
|             Color                                                           |
+=============================================================================+
*/


color_t to_move_color(const state_t *state) {
    return state->to_move_color;
}

/*
+=============================================================================+
|             Bitboards                                                       |
+=============================================================================+
*/

uint64_t states_color_bitboard(const state_t *state, color_t color) {
    uint64_t colorBitboard = 0;
    for (size_t i = PIECE_PAWN; i <= PIECE_KING; i++) {
        colorBitboard |= state->bitboards[color][i];
    }
    return colorBitboard;
}


/*
+=============================================================================+
|             Castling                                                       |
+=============================================================================+
*/

bool state_can_castle(const state_t *state, castle_t castle) {
    if (state->status != IN_GAME) return false;
}