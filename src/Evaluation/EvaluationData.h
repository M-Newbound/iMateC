/**
 * @file EvaluationData.h
 * @brief This file contains the declarations of the data used for evaluating the game state in the chess engine.
 * 
 * @details The PIECE_WEIGHT array holds the weights for each piece type. The STARTING_PIECE_WEIGHT constant represents the total weight of all pieces at the start of the game.
 * The PIECE_SQUARE_TABLES array contains piece-square tables for each piece type and game phase. These data are used in the evaluation of the game state.
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

#ifndef EVALUATION_DATA_H
#define EVALUATION_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Array of weights for each piece type.
 *
 * This array contains the weights for each piece type, used in the evaluation of the game state.
 * The indices correspond to the piece types (e.g., pawn, knight, bishop, rook, queen).
 */
extern const int PIECE_WEIGHT[5];

/**
 * @brief The total weight of all pieces at the start of the game.
 *
 * This constant is used in the evaluation of the game state to determine the phase of the game.
 */
extern const int STARTING_PIECE_WEIGHT;

/**
 * @brief Array of piece-square tables for each piece type and game phase.
 *
 * This array contains piece-square tables for each piece type and game phase (early and late game).
 * The piece-square table is used in the evaluation of the game state to determine the value of a piece based on its position on the board.
 * The first index corresponds to the piece type, the second index corresponds to the game phase (0 for early game, 1 for late game),
 * and the third index corresponds to the square on the board.
 */
extern const int PIECE_SQUARE_TABLES[6][2][64];

#ifdef __cplusplus
}
#endif
#endif