/* iMate -- Copyright (C) 2024 Martin Newbound */                                                    

/**
 * @file 
 * @brief
 * 
 * @details

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

#ifndef SEARCH_H
#define SEARCH_H

#include "../State/GameState.h"
#include "../Moves/Move.h"

/**
 * @brief Performs a search to find the best move from the current game state.
 *
 * This function performs a depth-first search of the game tree to find the best move from the current game state.
 * The search is performed up to a specified maximum depth.
 *
 * @param[in] state Pointer to the game state.
 * @param[out] best_move Pointer to a move_t object where the best move will be stored.
 * @param[in] max_depth The maximum depth of the search.
 */
void do_move_search(const state_t *state, move_t *best_move, int max_depth);

#endif // SEARCH_H