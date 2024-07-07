/* iMate -- Copyright (C) 2024 Martin Newbound */


#include "../MoveGeneration.h"
#include <stdlib.h>

/**
 * @brief Handles the generation of castling moves.
 *
 * This function generates all possible castling moves for a given color, and adds them to a move collection.
 *
 * @param collection The move collection to add the moves to.
 * @param color_to_move The color of the player to move.
 * 
 * @todo Implement this function.
 */
void handle_castling(move_collection_t *collection, color_t color_to_move) {
    // Implementation goes here
}

/**
 * @brief Generates all possible king moves on a given square.
 *
 * This function generates all possible king moves on a given square, and adds them to a move collection.
 * It also handles the generation of castling moves.
 *
 * @param state The current game state.
 * @param collection The move collection to add the moves to.
 * @param square_key The key of the square the king is on.
 */
void gen_king_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key) {
    const color_t color_to_move = get_state_to_move_color(state);

    flags_t flags = {
        .castle = NULL_CASTLE,
        .double_pawn_push = false,
        .promotion_piece = false,
        .king_moved = true,
        .kingside_rook_moved = false,
        .queenside_rook_moved = false
    };

    // King move offsets (in terms of squares)
    int offsets[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    int num_offsets = sizeof(offsets) / sizeof(offsets[0]);

    for (int i = 0; i < num_offsets; i++) {
        int64_t to_square = square_key + offsets[i];

        // Check if the move is within the board and doesn't wrap around
        if (to_square >= 0 && to_square < 64 && abs((to_square % 8) - (square_key % 8)) <= 1) {
            move_t const *move = new_move(square_key, to_square, flags);
            push_move_to_collection(move, collection);
        }
    }

    handle_castling(collection, color_to_move);
}