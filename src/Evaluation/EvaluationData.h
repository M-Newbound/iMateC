/* iMate -- Copyright (C) 2024 Martin Newbound */

/*
 * EvaluationData.h - piece weights and piece-square tables.
 *
 * PIECE_SQUARE_TABLES is indexed [piece][phase][square], where phase 0 is
 * opening/middlegame and phase 1 is endgame. Squares are rank-8-first
 * (index 0 = a8, index 63 = h1) matching the conventional table layout.
 */

#ifndef EVALUATION_DATA_H
#define EVALUATION_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern const int PIECE_WEIGHT[5];
extern const int STARTING_PIECE_WEIGHT;
extern const int PIECE_SQUARE_TABLES[6][2][64];

#ifdef __cplusplus
}
#endif

#endif /* EVALUATION_DATA_H */
