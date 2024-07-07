/* iMate -- Copyright (C) 2024 Martin Newbound */                                                    

#define EARLY_GAME_INDEX 0
#define LATE_GAME_INDEX 1

#define POSSESION_SCORE() (float) (possesion_weights[WHITE] - possesion_weights[BLACK])
#define POSITIONAL_SCORE(X) (float) (positional_weights[WHITE][X] - positional_weights[BLACK][X])
#define INTERPOLATE(MIN, MAX, FACTOR) (1 - FACTOR) * MAX + FACTOR * MIN

#include "../Evaluation/Evaluation.h"
#include "../Evaluation/EvaluationData.h"
#include "stdlib.h"

#include <float.h>

float evaluate_state(const state_t *curr_state) {
    // Check game status
    if (is_checkmate(curr_state, WHITE)) return FLT_MAX;
    if (is_checkmate(curr_state, BLACK)) return FLT_MIN;

    // Initialize weights
    int positional_weights[2][2] = {{0, 0}, {0, 0}};
    int possesion_weights[2] = {0, 0};
    
    // Calculate weights for each square
    for (size_t square = 0; square < 64; ++square) {
        piece_t piece = get_piece_on_square(curr_state, square);
        color_t color = get_color_of_piece_on_square(curr_state, square);

        if (piece == NULL_PIECE) continue;
        if (piece != PIECE_KING) possesion_weights[color] += PIECE_WEIGHT[piece];
        positional_weights[color][EARLY_GAME_INDEX] += PIECE_SQUARE_TABLES[piece][EARLY_GAME_INDEX][square];
        positional_weights[color][LATE_GAME_INDEX] += PIECE_SQUARE_TABLES[piece][LATE_GAME_INDEX][square];
    }

    // Calculate scores
    float possesion_score = POSSESION_SCORE();
    float early_positional_score = POSITIONAL_SCORE(EARLY_GAME_INDEX);
    float late_positional_score  = POSITIONAL_SCORE(LATE_GAME_INDEX);

    // Calculate phase factor
    float phase_factor = (float) (possesion_weights[WHITE] + possesion_weights[BLACK]) / (2.0f * STARTING_PIECE_WEIGHT);

    // Interpolate positional score and add possesion score
    float evaluation = INTERPOLATE(early_positional_score, late_positional_score, phase_factor);
    evaluation += possesion_score;

    return evaluation;
}