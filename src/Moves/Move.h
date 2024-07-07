
/**
 * @file Move.h
 * @brief This file contains the declarations of the functions and data structures used for handling chess moves.
 * 
 * @details The move_t structure represents a chess move, and the flags_t structure contains flags for special move types.
 * 
 * @version 1.0.0
 * @author Martin Newbound
 * @date 2024
 * 
 * @note License:
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef MOVE_H
#define MOVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../State/GameState.h"

// Represents a chess move
typedef struct move move_t;

// Contains flags for special move types
typedef struct flags {
    piece_t promotion_piece;
    castle_t castle;
    uint64_t en_passant_square;
    bool double_pawn_push;
    bool king_moved;
    bool kingside_rook_moved;
    bool queenside_rook_moved;
} flags_t;

/**
 * Creates a new move.
 * 
 * @param from_square The square the piece is moving from.
 * @param to_square The square the piece is moving to.
 * @param flags The flags for the move.
 * @return A pointer to the new move.
 */
move_t *new_move(uint64_t from_square, uint64_t to_square, flags_t flags);

/**
 * Frees a move.
 * 
 * @param move The move to free.
 */
void free_move(move_t *move);

/**
 * Applies a move to a game state.
 * 
 * @param move The move to apply.
 * @param state The game state to apply the move to.
 */
void apply_move(const move_t *move, state_t *state);

/**
 * Checks if a move is legal.
 * 
 * @param state The current game state.
 * @param move The move to check.
 * @return true if the move is legal, false otherwise.
 */
bool is_legal_move(const state_t *state, const move_t *move);

/**
 * Gets the flags of a move.
 * 
 * @param move The move to get the flags from.
 * @return A pointer to the flags of the move.
 */
const flags_t *get_move_flags(const move_t *move);

/**
 * Gets the from square of a move.
 * 
 * @param move The move to get the from square from.
 * @return The from square of the move.
 */
uint64_t get_move_from_square(const move_t *move);

/**
 * Gets the to square of a move.
 * 
 * @param move The move to get the to square from.
 * @return The to square of the move.
 */
uint64_t get_move_to_square(const move_t *move);


#ifdef __cplusplus
}
#endif

#endif // MOVE_H