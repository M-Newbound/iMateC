/* iMate -- Copyright (C) 2024 Martin Newbound */

/*
 * Perft counts leaf nodes at a given depth. If the numbers match the
 * reference values, move generation is correct - there's no ambiguity.
 * Values from https://www.chessprogramming.org/Perft_Results
 */

#include "framework.h"
#include "State/GameState.h"
#include "Moves/MoveCollection.h"
#include "Moves/MoveGeneration.h"
#include "Moves/MagicBitboards.h"
#include <stdint.h>


static uint64_t perft(state_t *state, int depth) {
    if (depth == 0) return 1;

    move_collection_t *moves = get_legal_moves_of_state(state);
    uint64_t count = 0;
    move_t  *m;

    while ((m = pop_collection_head(moves)) != NULL) {
        state_t *next = clone_state(state);
        play_move(next, m);
        count += perft(next, depth - 1);
        free_state(next);
        free_move(m);
    }
    free_move_collection(moves);
    return count;
}

/* Starting position - depth 1:20  2:400  3:8902  4:197281 */

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

static void test_start_depth1(void) {
    state_t *s = new_state(); load_fen_string(s, START_FEN);
    CHECK_EQ(perft(s, 1), (uint64_t)20);
    free_state(s);
}
static void test_start_depth2(void) {
    state_t *s = new_state(); load_fen_string(s, START_FEN);
    CHECK_EQ(perft(s, 2), (uint64_t)400);
    free_state(s);
}
static void test_start_depth3(void) {
    state_t *s = new_state(); load_fen_string(s, START_FEN);
    CHECK_EQ(perft(s, 3), (uint64_t)8902);
    free_state(s);
}
static void test_start_depth4(void) {
    state_t *s = new_state(); load_fen_string(s, START_FEN);
    CHECK_EQ(perft(s, 4), (uint64_t)197281);
    free_state(s);
}

/* Kiwipete - lots of castling, captures and promotions. depth 1:48  2:2039  3:97862 */

#define KIWI_FEN "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

static void test_kiwi_depth1(void) {
    state_t *s = new_state(); load_fen_string(s, KIWI_FEN);
    CHECK_EQ(perft(s, 1), (uint64_t)48);
    free_state(s);
}
static void test_kiwi_depth2(void) {
    state_t *s = new_state(); load_fen_string(s, KIWI_FEN);
    CHECK_EQ(perft(s, 2), (uint64_t)2039);
    free_state(s);
}
static void test_kiwi_depth3(void) {
    state_t *s = new_state(); load_fen_string(s, KIWI_FEN);
    CHECK_EQ(perft(s, 3), (uint64_t)97862);
    free_state(s);
}

/* Position 3 - promotion and en-passant stress. depth 1:14  2:191  3:2812 */

#define POS3_FEN "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"

static void test_pos3_depth1(void) {
    state_t *s = new_state(); load_fen_string(s, POS3_FEN);
    CHECK_EQ(perft(s, 1), (uint64_t)14);
    free_state(s);
}
static void test_pos3_depth2(void) {
    state_t *s = new_state(); load_fen_string(s, POS3_FEN);
    CHECK_EQ(perft(s, 2), (uint64_t)191);
    free_state(s);
}
static void test_pos3_depth3(void) {
    state_t *s = new_state(); load_fen_string(s, POS3_FEN);
    CHECK_EQ(perft(s, 3), (uint64_t)2812);
    free_state(s);
}

/* Position 4 - castling symmetry. depth 1:26  2:568  3:13744 */

#define POS4_FEN "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"

static void test_pos4_depth1(void) {
    state_t *s = new_state(); load_fen_string(s, POS4_FEN);
    CHECK_EQ(perft(s, 1), (uint64_t)26);
    free_state(s);
}
static void test_pos4_depth2(void) {
    state_t *s = new_state(); load_fen_string(s, POS4_FEN);
    CHECK_EQ(perft(s, 2), (uint64_t)568);
    free_state(s);
}
static void test_pos4_depth3(void) {
    state_t *s = new_state(); load_fen_string(s, POS4_FEN);
    CHECK_EQ(perft(s, 3), (uint64_t)13744);
    free_state(s);
}


int main(void) {
    magic_init();

    test_start_depth1();
    test_start_depth2();
    test_start_depth3();
    test_start_depth4();

    test_kiwi_depth1();
    test_kiwi_depth2();
    test_kiwi_depth3();

    test_pos3_depth1();
    test_pos3_depth2();
    test_pos3_depth3();

    test_pos4_depth1();
    test_pos4_depth2();
    test_pos4_depth3();

    RESULT();
}
