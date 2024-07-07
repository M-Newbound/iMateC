/* iMate -- Copyright (C) 2024 Martin Newbound */


#include "../MoveGeneration.h"

// Define masks for the king and queen side rooks for both colors
#define KINGSIDE_ROOK_MASK(COLOR) ((COLOR == WHITE) ? 0x0000000000000080 : 0x8000000000000000)
#define QUEENSIDE_ROOK_MASK(COLOR) ((COLOR == WHITE) ? 0x0000000000000010 : 0x1000000000000000)

// Define a macro to shift a bitboard in a given direction
#define SHIFT_IN_DIRECTION(BITBOARD, DIRECTION) (DIRECTION < 0) ? (BITBOARD << -DIRECTION) : (BITBOARD >> -DIRECTION)

/**
 * Creates flags for a rook move.
 * 
 * @param from_square The square the rook is moving from.
 * @param get_state_to_move_color The color of the rook.
 * @return The flags for the move.
 */
flags_t create_rook_flags(uint64_t from_square, color_t get_state_to_move_color) {
    flags_t flags = {
        .castle = NULL_CASTLE,
        .double_pawn_push = false,
        .promotion_piece = NULL_PIECE,
        .king_moved = false,
        .kingside_rook_moved = KINGSIDE_ROOK_MASK(get_state_to_move_color) & from_square,
        .queenside_rook_moved = QUEENSIDE_ROOK_MASK(get_state_to_move_color) & from_square
    };

    return flags;
}

/**
 * Handles a rook move or capture.
 * 
 * @param collection The collection of moves.
 * @param square_key The key of the square where the rook is located.
 * @param opponent_bitboard The bitboard of the opponent's pieces.
 * @param color_to_move The color of the rook.
 * @param shift_direction The direction to shift the bitboard.
 */
void handle_rook_move_or_capture(move_collection_t *collection, uint64_t square_key, uint64_t opponent_bitboard, color_t color_to_move, int shift_direction) {
    const uint64_t rank_mask = 0xFF << (8 * (square_key / 8));
    const uint64_t file_mask = 0x0101010101010101ULL << (square_key % 8);

    const uint64_t constraint_mask = rank_mask | file_mask;

    uint64_t next_move = SHIFT_IN_DIRECTION(next_move, shift_direction);

    while (next_move & constraint_mask > 0) {
        flags_t flags = create_rook_flags(square_key, color_to_move);
        move_t const *move = new_move(square_key, next_move, flags);
        push_move_to_collection(move, collection);
        
        if (next_move & opponent_bitboard > 0) break;
        next_move = SHIFT_IN_DIRECTION(next_move, shift_direction);
    }   
}

/**
 * Generates all possible moves for a rook on a given square.
 * 
 * @param state The current state of the game.
 * @param collection The collection of moves.
 * @param square_key The key of the square where the rook is located.
 */
void gen_rook_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key) {
    const color_t color_to_move = get_state_to_move_color(state);
    const uint64_t opponent_bitboard = states_color_bitboard(state, (color_to_move == WHITE) ? BLACK : WHITE);

    int directions[] = {8, -8, 1, -1};
    int num_directions = sizeof(directions) / sizeof(directions[0]);

    for (int i = 0; i < num_directions; i++) {
        handle_rook_move_or_capture(collection, square_key, opponent_bitboard, color_to_move, directions[i]);
    }
}