/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "MoveGeneration.h"
#include <stdlib.h>

// per-piece generators defined in PieceMoveGeneration/
void gen_pawn_moves_on_square(const state_t *state, move_collection_t *col, uint64_t sq_bb);
void gen_rook_moves_on_square(const state_t *state, move_collection_t *col, uint64_t sq_bb);
void gen_knight_moves_on_square(const state_t *state, move_collection_t *col, uint64_t sq_bb);
void gen_bishop_moves_on_square(const state_t *state, move_collection_t *col, uint64_t sq_bb);
void gen_queen_moves_on_square(const state_t *state, move_collection_t *col, uint64_t sq_bb);
void gen_king_moves_on_square(const state_t *state, move_collection_t *col, uint64_t sq_bb);
// no-castling variant used by the attack map to avoid re-entrant recursion
void gen_king_attacks_on_square(const state_t *state, move_collection_t *col, uint64_t sq_bb);


// generates pseudo-legal moves for target_color
// for_attacks=true skips castling to prevent infinite recursion through the attack map
static move_collection_t *generate_moves(const state_t *state,
                                         color_t target_color,
                                         bool for_attacks) {
    const state_t *s = state;
    state_t *owned = NULL;

    if (side_to_move(state) != target_color) {
        owned = clone_state(state);
        set_side_to_move(owned, target_color);
        s = owned;
    }

    move_collection_t *col = new_move_collection();
    uint64_t target_bb = color_bb(s, target_color);

    for (int i = 0; i < 64; i++) {
        uint64_t sq = 1ULL << i;
        if (!(target_bb & sq)) continue;

        switch (get_piece_on_square(s, sq)) {
            case PIECE_PAWN:   gen_pawn_moves_on_square(s, col, sq);   break;
            case PIECE_ROOK:   gen_rook_moves_on_square(s, col, sq);   break;
            case PIECE_KNIGHT: gen_knight_moves_on_square(s, col, sq); break;
            case PIECE_BISHOP: gen_bishop_moves_on_square(s, col, sq); break;
            case PIECE_QUEEN:  gen_queen_moves_on_square(s, col, sq);  break;
            case PIECE_KING:
                if (for_attacks) gen_king_attacks_on_square(s, col, sq);
                else             gen_king_moves_on_square(s, col, sq);
                break;
            default: break;
        }
    }

    if (owned) free_state(owned);
    return col;
}


// apply a move to a clone and check whether the mover's king ends up in check
static bool is_legal_move(const state_t *state, const move_t *move) {
    color_t mover = side_to_move(state);
    state_t *tmp = clone_state(state);
    play_move(tmp, move);
    bool legal = !is_check(tmp, mover);
    free_state(tmp);
    return legal;
}


// filter out moves that leave the king in check
static void prune_illegal_moves(const state_t *state, move_collection_t *col) {
    move_collection_t *legal = new_move_collection();
    move_t *move;
    while ((move = pop_collection_head(col)) != NULL) {
        if (is_legal_move(state, move))
            push_move_to_collection(move, legal);
        else
            free_move(move);
    }
    while ((move = pop_collection_head(legal)) != NULL)
        push_move_to_collection(move, col);
    free_move_collection(legal);
}


move_collection_t *get_legal_moves_of_state(const state_t *state) {
    move_collection_t *col = generate_moves(state, side_to_move(state), false);
    prune_illegal_moves(state, col);
    return col;
}

uint64_t get_attacked_squares_bitboard(const state_t *state) {
    color_t mover = side_to_move(state);
    color_t attacker = (mover == WHITE) ? BLACK : WHITE;

    move_collection_t *col = generate_moves(state, attacker, true);

    uint64_t attacked = 0;
    move_t *move;
    while ((move = pop_collection_head(col)) != NULL) {
        attacked |= get_move_to_square(move);
        free_move(move);
    }
    free_move_collection(col);
    return attacked;
}
