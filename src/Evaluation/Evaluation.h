/**
 * iMate -- Copyright (C) 2024 Martin Newbound
 * 
 * @file Evaluation.h
 * @brief Evaluation module for the iMate chess engine.
 * 
 * @details
 * The Evaluation module provides the capability to assess the current state of a chess game and 
 * quantify it as a single score. This is needed for decision making in zero-sum games.
 *
 * Without this module, the engine would not be able to differentiate between good and bad game states,
 * making it impossible to make strategic decisions. The evaluation function encapsulates the
 * knowledge and strategy of the chess engine, determining how it analyzes game states.
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


#ifndef EVALUATION_H
#define EVALUATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../State/GameState.h"

/**
 * @brief Evaluates a game state and returns an appropriate score.
 *
 * @details
 * The score represents the quality of a game state in perspective of the current player.
 * The higher a score, the better that state is for the current player.
 * 
 * The score is not assumed to be linear or restricted to any particular range.
 * However, the score is totally orderable such that a state with higher score is
 * better for the current player than a lower scored state.
 * 
 * @note The returned score can be any value in the domain of floating point numbers. 
 * @note A score which is the maximum or minimum possible value represents a checkmate.
 * @note Stalemates are handled on a context-specific basis and are not necessarily good or bad.
 * @note This function is deterministic.
 * 
 * @warning this function can only provide approximations of the true value of a game state. 
 *
 * @param state Pointer to the game state to evaluate.
 * @return Score of the given game state.
 */
float evaluate_state(const state_t *state);

#ifdef __cplusplus
}
#endif
#endif //EVALUATION_H