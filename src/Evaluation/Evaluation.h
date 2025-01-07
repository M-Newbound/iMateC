/* iMate -- Copyright (C) 2024 Martin Newbound */

/*
 * Evaluation.h - static position scoring.
 *
 * evaluate_state returns a score from the perspective of the side to move:
 * positive means the position is good for that side, negative means its bad.
 * The scale is centipawns (roughly). Checkmate returns +/-FLT_MAX.
 */

#ifndef EVALUATION_H
#define EVALUATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../State/GameState.h"

float evaluate_state(const state_t *state);

#ifdef __cplusplus
}
#endif

#endif /* EVALUATION_H */
