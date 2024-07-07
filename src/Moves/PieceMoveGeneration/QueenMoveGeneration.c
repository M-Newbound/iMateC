/* iMate -- Copyright (C) 2024 Martin Newbound */


#include "../MoveGeneration.h"

// Forward declarations
void gen_rook_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key);
void gen_bishop_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key);

/**
 * Generates all possible moves for a queen on a given square.
 * The queen's movement is a combination of a rook's and a bishop's movements,
 * so this function simply calls the functions to generate rook and bishop moves.
 * 
 * @param state The current state of the game.
 * @param collection The collection of moves.
 * @param square_key The key of the square where the queen is located.
 */
void gen_queen_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key) {
    // treat as rook and bishop combined
    gen_rook_moves_on_square(state, collection, square_key);
    gen_bishop_moves_on_square(state, collection, square_key);
}