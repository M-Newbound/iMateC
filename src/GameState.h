/**
 * iMate -- Copyright (C) 2024 Martin Newbound
 * 
 * @file GameState.h
 * @brief Module for managing the state of a chess game.
 * 
 * @details
 * This module defines the data structures and functions essential for representing
 * and manipulating the state of a chess game.
 * 
 * It encompasses definitions for player colors, piece types,
 * castling types, and the game status.
 * 
 * The `state_t` type encapsulates a specific state of a chess game. The functions
 * provided in this module facilitate the creation, modification, and querying of the game state.
 * 
 * @version 1.0.0
 * @author Martin Newbound
 * @date 2024
 * 
 * @note
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef  GAME_STATE_H
#define  GAME_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


typedef enum {
    WHITE           = 0,
    BLACK           = 1
} color_t;

typedef enum {
    NO_PIECE        = -1,
    PIECE_PAWN      = 0,
    PIECE_KNIGHT    = 1,
    PIECE_BISHOP    = 2,
    PIECE_ROOK      = 3,
    PIECE_QUEEN     = 4,
    PIECE_KING      = 5
} piece_t;

typedef enum {
    CASTLE_KINGSIDE_WHITE   = 0,
    CASTLE_QUEENSIDE_WHITE  = 1,
    CASTLE_KINGSIDE_BLACK   = 2,
    CASTLE_QUEENSIDE_BLACK  = 3
} castle_t;

typedef enum {
    WHITE_CHECK,
    BLACK_CHECK,
    WHITE_CHECKMATE,
    BLACK_CHECKMATE,
    STALEMATE,
    IN_GAME
} game_status_t;


/**
 * @brief Forward declaration of the state type
 * 
 * @details
 * The inner workings of the state struct are hidden as
 * they are not relevant to the public interface of this module
 * 
 * However, this type represents and encapsulates any given state
 * of a chess game. And can be used to manipulate, query, and analyze
 * the state of a chess game.
 */
typedef struct state state_t;


/**
 * @brief Allocates and initializes a new game state.
 *
 * @details
 * This function allocates memory for a new game state.
 * The new state is initialized to a default "null" like state. Which
 * means it has no pieces on the board and all castling rights are enabled.
 * 
 * Hence the state can not be used imediatly for a new chess game. It's starting
 * state must be setup independantly.
 * 
 * @warning The caller is responsible for freeing the memory allocated by this function.
 * @see free_state
 * @see state_t
 *
 * @return Pointer to the new game state.
 */
state_t* new_state();


/**
 * @brief Frees the memory allocated for a game state.
 *
 * @details
 * This function frees the memory allocated for a game state by the new_state function.
 * It should be called when the game state is no longer needed to prevent memory leaks.
 * 
 * @warning after freeing a game state, it should not be used anymore. As this will result in undefined behavior.
 *
 * @param state Pointer to the game state to be freed.
 */
void free_state(state_t *state);


/**
 * @brief Returns the current status of the game.
 *
 * @details
 * This function checks the current position and returns the game status.
 *
 * @param state Pointer to the game state.
 * @return The current game status.
 */
game_status_t current_state_status(const state_t *state);


/**
 * @brief Returns the color of the current player to move.
 *
 * @details
 * This function returns the color of the player whose turn it is to move in the current state.
 *
 * @param state Pointer to the game state.
 * @return The color of the player to move.
 */
color_t to_move_color(const state_t *state);


/**
 * @brief Copies the game state from one state type to another.
 *
 * @details
 * This function is used to create a duplicate of a game state. This is useful
 * when you want to make changes to the game state, but also need to keep
 * the original state unchanged for comparison or rollback purposes.
 * 
 * This is a deep copy function, meaning that all/any attributes of the game state are
 * recursivley copied (if need be).
 *
 * @param fromState     Pointer to the source game state.
 * @param toState       Pointer to the destination game state.
 */
void copy_state(const state_t *fromState, state_t *toState);


/**
 * @brief Returns the piece on a given square.
 *
 * @details
 * This function checks the game state and returns the piece on the given square. 
 * If the square is empty, it returns NO_PIECE.
 * 
 * @note The square is represented as a 64-bit integer with the bit corresponding to the square set to 1.
 * @warning The square must be a valid square mask (i.e. a single bit set to 1). Else the behavior is undefined.
 *
 * @param state Pointer to the game state.
 * @param square The square to check.
 * @return The piece on the given square.
 */
piece_t piece_on_square(const state_t* state, uint64_t square);


/**
 * @brief Retrieves the castling right for a specific type of castling.
 *
 * @details
 * In chess, castling is a special move that involves the player's king and one of their rooks.
 * There are two types of castling: kingside and queenside. This function retrieves the castling
 * right for a specific type of castling.
 *
 * This function is needed to check if a player is allowed to castle at a given point in the game.
 * Castling rights can be lost if the king or the rook has moved, or if the king is in check.
 * 
 * @warning this represents sudo castling rights, it does not handle the case where the king is in check.
 *
 * @param state     Pointer to the game state.
 * @param castle    The type of castling to check the right for.
 * 
 * @return          true if the player has the right to castle, false otherwise.
 */
bool state_can_castle(const state_t *state, castle_t castle);


/**
 * @brief Retrieves the bitboard representing the positions of all pieces of a specific color.
 *
 * @details
 * A bitboard is a data structure used in computer systems that play board games, where each bit corresponds
 * to a game board space or piece. This function retrieves the bitboard for a specific color.
 *
 * @param state     Pointer to the game state.
 * @param color     The color of the pieces to get the bitboard for.
 * 
 * @return          The bitboard for the specified color.
 */
uint64_t states_color_bitboard(const state_t *state, color_t color);


/**
 * @brief Retrieves the bitboard representing the positions of a specific type of piece of a specific color.
 *
 * @details
 * This function retrieves the bitboard for a specific type of piece of a specific color.
 *
 * @param state     Pointer to the game state.
 * @param piece     The type of piece to get the bitboard for.
 * @param color     The color of the pieces to get the bitboard for.
 * 
 * @return          The bitboard for the specified piece and color.
 */
uint64_t states_piece_bitboard(const state_t *state, piece_t piece, color_t color);


/**
 * @brief Sets the bitboard for a specific type of piece of a specific color.
 *
 * @details
 * This function is used to update the game state when a piece is moved or removed from the game.
 *
 * @param state         Pointer to the game state.
 * @param piece         The type of piece to set the bitboard for.
 * @param color         The color of the pieces to set the bitboard for.
 * @param bitboard      The new bitboard for the specified piece and color.
 */
void set_states_piece_bitboard(state_t *state, piece_t piece, color_t color, uint64_t bitboard);


#ifdef __cplusplus
}
#endif

#endif // GAME_STATE_H