/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "framework.h"
#include "State/GameState.h"
#include "Moves/Move.h"
#include "Moves/MoveCollection.h"
#include "Moves/MoveGeneration.h"
#include "Moves/MagicBitboards.h"

/* Convenience: square bitboard from algebraic notation. */
static uint64_t sq(char file, int rank) {
    return 1ULL << ((rank - 1) * 8 + (file - 'a'));
}

#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

/* lifecycle */

static void test_new_state_is_empty(void) {
    state_t *s = new_state();
    CHECK_NOT_NULL(s);
    /* No pieces on any square */
    for (int r = 1; r <= 8; r++)
        for (char f = 'a'; f <= 'h'; f++)
            CHECK_EQ(get_piece_on_square(s, sq(f, r)), NULL_PIECE);
    CHECK_EQ(side_to_move(s), WHITE);
    free_state(s);
}

static void test_clone_is_independent(void) {
    state_t *orig = new_state();
    load_fen_string(orig, STARTING_FEN);
    state_t *copy = clone_state(orig);

    /* Modify copy - original should be unchanged */
    set_side_to_move(copy, BLACK);
    CHECK_EQ(side_to_move(orig), WHITE);
    CHECK_EQ(side_to_move(copy), BLACK);

    free_state(orig);
    free_state(copy);
}

/* FEN loading */

static void test_starting_fen_pieces(void) {
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);

    /* White pieces on rank 1 */
    CHECK_EQ(get_piece_on_square(s, sq('a', 1)), PIECE_ROOK);
    CHECK_EQ(get_piece_on_square(s, sq('b', 1)), PIECE_KNIGHT);
    CHECK_EQ(get_piece_on_square(s, sq('c', 1)), PIECE_BISHOP);
    CHECK_EQ(get_piece_on_square(s, sq('d', 1)), PIECE_QUEEN);
    CHECK_EQ(get_piece_on_square(s, sq('e', 1)), PIECE_KING);
    CHECK_EQ(get_piece_on_square(s, sq('f', 1)), PIECE_BISHOP);
    CHECK_EQ(get_piece_on_square(s, sq('g', 1)), PIECE_KNIGHT);
    CHECK_EQ(get_piece_on_square(s, sq('h', 1)), PIECE_ROOK);
    /* Colors on rank 1 */
    for (char f = 'a'; f <= 'h'; f++)
        CHECK_EQ(get_color_of_piece_on_square(s, sq(f, 1)), WHITE);

    /* Black pawns on rank 7 */
    for (char f = 'a'; f <= 'h'; f++) {
        CHECK_EQ(get_piece_on_square(s, sq(f, 7)), PIECE_PAWN);
        CHECK_EQ(get_color_of_piece_on_square(s, sq(f, 7)), BLACK);
    }

    /* Empty ranks 3–6 */
    for (int r = 3; r <= 6; r++)
        for (char f = 'a'; f <= 'h'; f++)
            CHECK_EQ(get_piece_on_square(s, sq(f, r)), NULL_PIECE);

    free_state(s);
}

static void test_starting_fen_side_to_move(void) {
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);
    CHECK_EQ(side_to_move(s), WHITE);
    free_state(s);
}

static void test_starting_fen_castling_rights(void) {
    state_t *s = new_state();
    /* state_can_castle checks BOTH rights AND path clearance.
     * In the starting position all castling paths are blocked by pieces. */
    load_fen_string(s, STARTING_FEN);
    CHECK_FALSE(state_can_castle(s, CASTLE_KINGSIDE_WHITE));
    CHECK_FALSE(state_can_castle(s, CASTLE_QUEENSIDE_WHITE));
    CHECK_FALSE(state_can_castle(s, CASTLE_KINGSIDE_BLACK));
    CHECK_FALSE(state_can_castle(s, CASTLE_QUEENSIDE_BLACK));
    free_state(s);

    /* With a clear path and rights, castling should be available */
    state_t *s2 = new_state();
    load_fen_string(s2, "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    CHECK_TRUE(state_can_castle(s2, CASTLE_KINGSIDE_WHITE));
    CHECK_TRUE(state_can_castle(s2, CASTLE_QUEENSIDE_WHITE));
    free_state(s2);
}

static void test_fen_no_castling_rights(void) {
    state_t *s = new_state();
    load_fen_string(s, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1");
    CHECK_FALSE(state_can_castle(s, CASTLE_KINGSIDE_WHITE));
    CHECK_FALSE(state_can_castle(s, CASTLE_QUEENSIDE_WHITE));
    CHECK_FALSE(state_can_castle(s, CASTLE_KINGSIDE_BLACK));
    CHECK_FALSE(state_can_castle(s, CASTLE_QUEENSIDE_BLACK));
    free_state(s);
}

static void test_fen_en_passant_target(void) {
    /* After 1.e4 - ep target is e3 */
    state_t *s = new_state();
    load_fen_string(s, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    CHECK_TRUE(is_en_passant_target_active(s));
    CHECK_EQ(get_en_passant_target(s), sq('e', 3));
    free_state(s);
}

static void test_fen_black_to_move(void) {
    state_t *s = new_state();
    load_fen_string(s, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    CHECK_EQ(side_to_move(s), BLACK);
    free_state(s);
}

/* play_move */

static void test_play_pawn_single_push(void) {
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);

    flags_t f = {NULL_PIECE, NULL_CASTLE, 0, false, false, false};
    move_t *m = new_move(sq('e', 2), sq('e', 3), f);
    play_move(s, m);
    free_move(m);

    CHECK_EQ(get_piece_on_square(s, sq('e', 2)), NULL_PIECE);
    CHECK_EQ(get_piece_on_square(s, sq('e', 3)), PIECE_PAWN);
    CHECK_EQ(get_color_of_piece_on_square(s, sq('e', 3)), WHITE);
    CHECK_EQ(side_to_move(s), BLACK);
    free_state(s);
}

static void test_play_pawn_double_push_sets_ep(void) {
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);

    flags_t f = {NULL_PIECE, NULL_CASTLE, sq('e', 3), false, false, false};
    move_t *m = new_move(sq('e', 2), sq('e', 4), f);
    play_move(s, m);
    free_move(m);

    CHECK_TRUE(is_en_passant_target_active(s));
    CHECK_EQ(get_en_passant_target(s), sq('e', 3));
    free_state(s);
}

static void test_play_kingside_castle_white(void) {
    /* Position with clear path: king on e1, rook on h1, castling rights */
    state_t *s = new_state();
    load_fen_string(s, "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

    flags_t f = {NULL_PIECE, CASTLE_KINGSIDE_WHITE, 0, true, false, false};
    move_t *m = new_move(sq('e', 1), sq('g', 1), f);
    play_move(s, m);
    free_move(m);

    CHECK_EQ(get_piece_on_square(s, sq('e', 1)), NULL_PIECE);
    CHECK_EQ(get_piece_on_square(s, sq('g', 1)), PIECE_KING);
    CHECK_EQ(get_color_of_piece_on_square(s, sq('g', 1)), WHITE);
    CHECK_EQ(get_piece_on_square(s, sq('h', 1)), NULL_PIECE);
    CHECK_EQ(get_piece_on_square(s, sq('f', 1)), PIECE_ROOK);
    free_state(s);
}

static void test_play_en_passant_removes_pawn(void) {
    /* Black pawn on d4, white just pushed e2→e4 (ep target = e3) */
    state_t *s = new_state();
    load_fen_string(s,
        "rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

    flags_t f = {NULL_PIECE, NULL_CASTLE, 0, false, false, false};
    move_t *m = new_move(sq('d', 4), sq('e', 3), f);
    play_move(s, m);
    free_move(m);

    /* Capturing pawn is on e3 */
    CHECK_EQ(get_piece_on_square(s, sq('e', 3)), PIECE_PAWN);
    CHECK_EQ(get_color_of_piece_on_square(s, sq('e', 3)), BLACK);
    /* d4 is empty */
    CHECK_EQ(get_piece_on_square(s, sq('d', 4)), NULL_PIECE);
    /* White pawn on e4 is gone (captured en passant) */
    CHECK_EQ(get_piece_on_square(s, sq('e', 4)), NULL_PIECE);
    free_state(s);
}

static void test_play_promotion(void) {
    /* White pawn on e7 ready to promote */
    state_t *s = new_state();
    load_fen_string(s, "8/4P3/8/8/8/8/8/4K2k w - - 0 1");

    flags_t f = {PIECE_QUEEN, NULL_CASTLE, 0, false, false, false};
    move_t *m = new_move(sq('e', 7), sq('e', 8), f);
    play_move(s, m);
    free_move(m);

    CHECK_EQ(get_piece_on_square(s, sq('e', 8)), PIECE_QUEEN);
    CHECK_EQ(get_color_of_piece_on_square(s, sq('e', 8)), WHITE);
    CHECK_EQ(get_piece_on_square(s, sq('e', 7)), NULL_PIECE);
    free_state(s);
}

/* check / checkmate / stalemate */

static void test_starting_pos_no_check(void) {
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);
    CHECK_FALSE(is_check(s, WHITE));
    CHECK_FALSE(is_check(s, BLACK));
    free_state(s);
}

static void test_is_check_detected(void) {
    /* White queen on h5 checks black king on e8 */
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/8/7Q/8/8/8/4K3 b - - 0 1");
    CHECK_TRUE(is_check(s, BLACK));
    CHECK_FALSE(is_check(s, WHITE));
    free_state(s);
}

static void test_fools_mate_is_checkmate(void) {
    /* Fool's mate: 1.f3 e5 2.g4 Qh4# */
    state_t *s = new_state();
    load_fen_string(s,
        "rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3");
    CHECK_TRUE(is_check(s, WHITE));
    CHECK_TRUE(is_checkmate(s, WHITE));
    CHECK_FALSE(is_checkmate(s, BLACK));
    free_state(s);
}

static void test_stalemate_detected(void) {
    /* Black king on a8, white king on a6, white queen on b5: stalemate */
    state_t *s = new_state();
    load_fen_string(s, "k7/8/K7/1Q6/8/8/8/8 b - - 0 1");
    CHECK_FALSE(is_check(s, BLACK));
    CHECK_TRUE(is_stalemate(s, BLACK));
    CHECK_FALSE(is_checkmate(s, BLACK));
    free_state(s);
}

static void test_get_state_status_in_game(void) {
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);
    game_status_t st = get_state_status(s);
    CHECK_EQ(st, IN_GAME);
    free_state(s);
}

static void test_get_state_status_checkmate(void) {
    state_t *s = new_state();
    load_fen_string(s,
        "rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3");
    game_status_t st = get_state_status(s);
    CHECK_EQ(st, WHITE_CHECKMATE);
    free_state(s);
}


int main(void) {
    magic_init();
    test_new_state_is_empty();
    test_clone_is_independent();
    test_starting_fen_pieces();
    test_starting_fen_side_to_move();
    test_starting_fen_castling_rights();
    test_fen_no_castling_rights();
    test_fen_en_passant_target();
    test_fen_black_to_move();
    test_play_pawn_single_push();
    test_play_pawn_double_push_sets_ep();
    test_play_kingside_castle_white();
    test_play_en_passant_removes_pawn();
    test_play_promotion();
    test_starting_pos_no_check();
    test_is_check_detected();
    test_fools_mate_is_checkmate();
    test_stalemate_detected();
    test_get_state_status_in_game();
    test_get_state_status_checkmate();
    RESULT();
}
