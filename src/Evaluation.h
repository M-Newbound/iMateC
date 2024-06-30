/*---------------------------------------------------------------------\                                                                
| iMate -- Copyright (C) 2024 Martin Newbound                          |                                                          
|                                                                      |
| This program is free software: you can redistribute it and/or modify |
| it under the terms of the GNU General Public License as published by |
| the Free Software Foundation, either version 3 of the License, or    |
| (at your option) any later version.                                  |
|                                                                      |
| This program is distributed in the hope that it will be useful,      |
| but WITHOUT ANY WARRANTY; without even the implied warranty of       |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         |
| GNU General Public License for more details.                         |
|                                                                      |
| You should have received a copy of the GNU General Public License    |
| along with this program. If not, see https://www.gnu.org/licenses/   |
\---------------------------------------------------------------------*/

/**
 * @file Evaluation.h
 * @brief Evaluation module for the iMate chess engine.
 *
 * The Evaluation module provides the capability to assess the state of a chess game and 
 * quantify as a single score. This is needed for decision making in zero-sum games like chess
 *
 * Without this module, the engine would not be able to differentiate between good and bad game states,
 * making it impossible to make strategic decisions. The evaluation function encapsulates the
 * knowledge and strategy of the chess engine, determining how it plays the game.
 */

#ifndef EVALUATION_H
#define EVALUATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "GameState.h"

/**
 * @brief Evaluates the given game state and returns a score.
 *
 * The score represents the quality of the game state from the perspective of the current player.
 * A higher score means the state is more favorable for the current player.
 *
 * @param state Pointer to the game state to evaluate.
 * @return Score of the given game state.
 */
float evaluate_state(const state_t *state);


#ifdef __cplusplus
}
#endif
#endif //EVALUATION_H