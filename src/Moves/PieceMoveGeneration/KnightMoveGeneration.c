/* iMate -- Copyright (C) 2024 Martin Newbound */


#include "../MoveGeneration.h"
#include <stdlib.h>

/**
 * @brief Generates all possible knight moves on a given square.
 *
 * This function generates all possible knight moves on a given square, and adds them to a move collection.
 *
 * @param state The current game state.
 * @param collection The move collection to add the moves to.
 * @param square_key The key of the square the knight is on.
 */
void gen_knight_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key) {
    const color_t color_to_move = get_state_to_move_color(state);

    flags_t flags = {
        .castle = NULL_CASTLE,
        .double_pawn_push = false,
        .promotion_piece = false,
        .king_moved = false,
        .kingside_rook_moved = false,
        .queenside_rook_moved = false
    };

    // Knight move offsets (in terms of squares)
    int offsets[] = {-17, -15, -10, -6, 6, 10, 15, 17};
    int num_offsets = sizeof(offsets) / sizeof(offsets[0]);

    for (int i = 0; i < num_offsets; i++) {
        int64_t to_square = square_key + offsets[i];

        // Check if the move is within the board and doesn't wrap around
        if (to_square >= 0 && to_square < 64 && abs((to_square % 8) - (square_key % 8)) <= 2) {
            move_t const *move = new_move(square_key, to_square, flags);
            push_move_to_collection(move, collection);
        }
    }
}