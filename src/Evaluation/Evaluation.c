/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "Evaluation.h"
#include "EvaluationData.h"
#include <float.h>

#define MG 0  // midgame / opening
#define EG 1  // endgame

float evaluate_state(const state_t *state) {
    const color_t to_move = side_to_move(state);
    if (is_checkmate(state, to_move)) return -FLT_MAX;

    int pst[2][2] = {{0, 0}, {0, 0}};
    int material[2] = {0, 0};

    for (int sq_idx = 0; sq_idx < 64; sq_idx++) {
        uint64_t sq_bb = 1ULL << sq_idx;
        piece_t piece = get_piece_on_square(state, sq_bb);
        if (piece == NULL_PIECE) continue;
        color_t color = get_color_of_piece_on_square(state, sq_bb);

        if (piece != PIECE_KING)
            material[color] += PIECE_WEIGHT[piece];

        // tables are rank-8-first; flip for white so advancing toward rank 8
        // hits the high end of the table. black's index is already the right way round.
        int tbl_idx = (color == WHITE)
            ? (7 - sq_idx / 8) * 8 + (sq_idx % 8)
            : sq_idx;

        pst[color][MG] += PIECE_SQUARE_TABLES[piece][MG][tbl_idx];
        pst[color][EG] += PIECE_SQUARE_TABLES[piece][EG][tbl_idx];
    }

    // 1.0 = opening, 0.0 = pure endgame
    int total_material = material[WHITE] + material[BLACK];
    float phase = (float)total_material / (2.0f * STARTING_PIECE_WEIGHT);
    if (phase > 1.0f) phase = 1.0f;
    if (phase < 0.0f) phase = 0.0f;

    // compute from white's perspective, then flip for whoever is to move
    float mat_score = (float)(material[WHITE] - material[BLACK]);
    float pos_early = (float)(pst[WHITE][MG] - pst[BLACK][MG]);
    float pos_late  = (float)(pst[WHITE][EG] - pst[BLACK][EG]);

    float pos_score = phase * pos_early + (1.0f - phase) * pos_late;
    float score = mat_score + pos_score;

    return (to_move == WHITE) ? score : -score;
}
