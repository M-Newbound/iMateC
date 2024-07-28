/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "MoveGeneration.h"
#include <stdlib.h>

void gen_pawn_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key);

void gen_rook_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key);

void gen_knight_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key);

void gen_bishop_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key);

void gen_queen_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key);

void gen_king_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key);

bool is_legal_move(const state_t *state, const move_t *move) {
    color_t orig_color = get_state_to_move_color(state);
    state_t *temporary_state = new_state();
    copy_state(state, temporary_state);
    apply_move(move, temporary_state);

    bool is_legal = !is_check(state, orig_color);

    free_state(temporary_state);
    return is_legal;
}

void prune_illegal_moves(const state_t *state, move_collection_t *collection) {
    const move_t* head_move = pop_collection_head(collection);
    if (head_move == NULL) return;
    
    prune_illegal_moves(state, collection);
    if (is_legal_move(state, head_move)) push_move_to_collection(head_move, collection);
    else free_move(head_move);
}

move_collection_t *generate_psudo_legal_moves(const state_t *state, color_t target_color) {
    move_collection_t *collection = new_move_collection();

    for (size_t i = 0; i < 64; ++i) {
        const uint64_t square_key = 1 << i;

        switch(piece_on_square(state, square_key)) {
            case PIECE_PAWN:
                gen_pawn_moves_on_square(state, collection, square_key);
                break;

            case PIECE_ROOK:
                gen_rook_moves_on_square(state, collection, square_key);
                break;
            
            case PIECE_KNIGHT:
                gen_knight_moves_on_square(state, collection, square_key);
                break;

            case PIECE_BISHOP:
                gen_bishop_moves_on_square(state, collection, square_key);
                break;

            case PIECE_QUEEN:
                gen_queen_moves_on_square(state, collection, square_key);
                break;

            case PIECE_KING:
                gen_king_moves_on_square(state, collection, square_key);
                break;
            
            default:
                break;
        }
    }

    return collection;
}


move_collection_t *get_legal_moves_of_state(const state_t *state) {
    move_collection_t *collection = generate_psudo_legal_moves(state, get_state_to_move_color(state));
    prune_illegal_moves(state, collection);

    return collection;
}

uint64_t get_attacked_squares_bitboard(const state_t *state) {
    color_t target_color = get_state_to_move_color(state) == WHITE ? BLACK : WHITE;
    move_collection_t *collection = generate_psudo_legal_moves(state, target_color);

    uint64_t attacked_squares = 0;

    move_t *move = pop_collection_head(collection);
    while (move != NULL) {
        attacked_squares |= get_move_to_square(move);
        free_move(move);
        move = pop_collection_head(collection);
    }

    free_move_collection(collection);
}
