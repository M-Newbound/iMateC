/* iMate -- Copyright (C) 2024 Martin Newbound */                                                    

/**
 * @file MoveGeneration.h
 * @brief This file contains the declarations of the functions used for generating legal moves and attacked squares.
 * 
 * @details The get_legal_moves_of_state function generates a collection of all legal moves for a given game state.
 * The get_attacked_squares_bitboard function generates a bitboard of all squares attacked by a given game state.
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

#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include "../State/GameState.h"
#include "Move.h"
#include "MoveCollection.h"

/**
 * Generates a collection of all legal moves for a given game state.
 * 
 * @param state The game state to generate the legal moves for.
 * @return A pointer to the collection of legal moves.
 */
move_collection_t *get_legal_moves_of_state(const state_t *state);

/**
 * Generates a bitboard of all squares attacked by a given game state.
 * 
 * @param state The game state to generate the attacked squares bitboard for.
 * @return A bitboard of all squares attacked by the given game state.
 */
uint64_t get_attacked_squares_bitboard(const state_t *state);

#endif // MOVE_GEN_H