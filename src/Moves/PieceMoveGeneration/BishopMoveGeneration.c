/* iMate -- Copyright (C) 2024 Martin Newbound */


#include "../MoveGeneration.h"

#define SHIFT_IN_DIRECTION(BITBOARD, DIRECTION) (DIRECTION < 0) ? (BITBOARD << -DIRECTION) : (BITBOARD >> DIRECTION)

/**
 * @brief Handles the generation of bishop moves or captures in a specific direction.
 *
 * This function generates all possible bishop moves or captures in a specific direction from a given square,
 * and adds them to a move collection.
 *
 * @param collection The move collection to add the moves to.
 * @param square_key The key of the square the bishop is on.
 * @param opponent_bitboard The bitboard representing the opponent's pieces.
 * @param color_to_move The color of the player to move.
 * @param shift_direction The direction to shift the bishop's bitboard in.
 */
void handle_bishop_move_or_capture(move_collection_t *collection, uint64_t square_key, uint64_t opponent_bitboard, color_t color_to_move, int shift_direction) {
    const uint64_t diagonal_mask = 0x8040201008040201ULL << (square_key % 8);
    const uint64_t anti_diagonal_mask = 0x0102040810204080ULL << (7 - (square_key % 8));

    const uint64_t constraint_mask = diagonal_mask | anti_diagonal_mask;

    uint64_t next_move = SHIFT_IN_DIRECTION(1ULL << square_key, shift_direction);

    flags_t flags = {
        .castle = NULL_CASTLE,
        .double_pawn_push = false,
        .promotion_piece = false,
        .king_moved = false,
        .kingside_rook_moved = false,
        .queenside_rook_moved = false
    };

    while (next_move & constraint_mask) {
        move_t const *move = new_move(square_key, next_move, flags);
        push_move_to_collection(move, collection);
        
        if (next_move & opponent_bitboard) break;
        next_move = SHIFT_IN_DIRECTION(next_move, shift_direction);
    }   
}

/**
 * @brief Generates all possible bishop moves on a given square.
 *
 * This function generates all possible bishop moves on a given square, and adds them to a move collection.
 *
 * @param state The current game state.
 * @param collection The move collection to add the moves to.
 * @param square_key The key of the square the bishop is on.
 */
void gen_bishop_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key) {
    const color_t color_to_move = get_state_to_move_color(state);
    const uint64_t opponent_bitboard = states_color_bitboard(state, (color_to_move == WHITE) ? BLACK : WHITE);

    int directions[] = {9, -9, 7, -7};
    int num_directions = sizeof(directions) / sizeof(directions[0]);

    for (int i = 0; i < num_directions; i++) {
        handle_bishop_move_or_capture(collection, square_key, opponent_bitboard, color_to_move, directions[i]);
    }
}