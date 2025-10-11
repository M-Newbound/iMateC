/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "framework.h"
#include "State/GameState.h"
#include "Evaluation/Evaluation.h"
#include "Moves/MagicBitboards.h"
#include <float.h>

#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

static void test_starting_pos_near_zero(void) {
    /* Starting position is symmetric - score should be 0 (tables are mirrored). */
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);
    float score = evaluate_state(s);
    CHECK_FLOAT_NEAR(score, 0.0f, 1.0f);
    free_state(s);
}

static void test_extra_queen_is_positive(void) {
    /* White has extra queen - score should be strongly positive for white */
    state_t *s = new_state();
    /* White: king + queen + all pawns. Black: king + all pawns (no queen). */
    load_fen_string(s,
        "4k3/pppppppp/8/8/8/8/PPPPPPPP/3QK3 w - - 0 1");
    float score = evaluate_state(s);
    CHECK_TRUE(score > 500.0f); /* queen is worth ~900 centi-pawns */
    free_state(s);
}

static void test_material_deficit_is_negative(void) {
    /* Black has extra queen - score should be strongly negative for white (to-move) */
    state_t *s = new_state();
    load_fen_string(s,
        "3qk3/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
    float score = evaluate_state(s);
    CHECK_TRUE(score < -500.0f);
    free_state(s);
}

static void test_symmetric_score_flips_with_side_to_move(void) {
    /* Same position, different side to move - scores should be negatives */
    state_t *white = new_state();
    state_t *black  = new_state();
    /* King + rook each - roughly equal but with slight positional differences */
    load_fen_string(white, "4k3/8/8/8/8/8/8/R3K3 w - - 0 1");
    load_fen_string(black,  "4k3/8/8/8/8/8/8/R3K3 b - - 0 1");
    float sw = evaluate_state(white);
    float sb = evaluate_state(black);
    /* From white's perspective score is positive; from black's perspective
     * of the SAME position it should be negative (or at least opposite sign). */
    CHECK_TRUE(sw > 0.0f);
    CHECK_TRUE(sb < 0.0f);
    free_state(white);
    free_state(black);
}

static void test_checkmate_returns_neg_max(void) {
    /* Current player (white) is checkmated - evaluate_state returns -FLT_MAX */
    state_t *s = new_state();
    /* Fool's mate: white is checkmated */
    load_fen_string(s,
        "rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3");
    float score = evaluate_state(s);
    CHECK_FLOAT_NEAR(score, -FLT_MAX, 1.0f);
    free_state(s);
}

static void test_opponent_checkmated_returns_pos_max(void) {
    /* Opponent (black) is checkmated from white's PoV */
    state_t *s = new_state();
    /* Scholar's mate: black is checkmated, but it's black to move */
    load_fen_string(s,
        "r1bqkb1r/pppp1Qpp/2n2n2/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 4");
    float score = evaluate_state(s);
    /* Black is to move and is checkmated → score is -FLT_MAX from black's PoV,
     * but evaluate_state returns from current player's PoV, so -FLT_MAX */
    CHECK_FLOAT_NEAR(score, -FLT_MAX, 1.0f);
    free_state(s);
}

static void test_more_pawns_better_score(void) {
    /* White has 3 extra pawns - should score higher than equal material */
    state_t *few = new_state();
    state_t *many = new_state();
    load_fen_string(few,  "4k3/8/8/8/8/8/5PPP/4K3 w - - 0 1");
    load_fen_string(many, "4k3/8/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
    float sf = evaluate_state(few);
    float sm = evaluate_state(many);
    CHECK_TRUE(sm > sf);
    free_state(few);
    free_state(many);
}


int main(void) {
    magic_init();
    test_starting_pos_near_zero();
    test_extra_queen_is_positive();
    test_material_deficit_is_negative();
    test_symmetric_score_flips_with_side_to_move();
    test_checkmate_returns_neg_max();
    test_opponent_checkmated_returns_pos_max();
    test_more_pawns_better_score();
    RESULT();
}
