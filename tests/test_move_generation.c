/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "framework.h"
#include "State/GameState.h"
#include "Moves/Move.h"
#include "Moves/MoveCollection.h"
#include "Moves/MoveGeneration.h"
#include "Moves/MagicBitboards.h"


static uint64_t sq(char file, int rank) {
    return 1ULL << ((rank - 1) * 8 + (file - 'a'));
}

static int count_legal_moves(const state_t *state) {
    move_collection_t *moves = get_legal_moves_of_state(state);
    int n = 0;
    move_t *m;
    while ((m = pop_collection_head(moves)) != NULL) { free_move(m); n++; }
    free_move_collection(moves);
    return n;
}

static int count_legal_moves_from(const state_t *state, uint64_t from) {
    move_collection_t *moves = get_legal_moves_of_state(state);
    int n = 0;
    move_t *m;
    while ((m = pop_collection_head(moves)) != NULL) {
        if (get_move_from_square(m) == from) n++;
        free_move(m);
    }
    free_move_collection(moves);
    return n;
}

static bool has_move(const state_t *state, uint64_t from, uint64_t to) {
    move_collection_t *moves = get_legal_moves_of_state(state);
    bool found = false;
    move_t *m;
    while ((m = pop_collection_head(moves)) != NULL) {
        if (get_move_from_square(m) == from && get_move_to_square(m) == to)
            found = true;
        free_move(m);
    }
    free_move_collection(moves);
    return found;
}

#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"


static void test_starting_pos_20_moves(void) {
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);
    CHECK_EQ(count_legal_moves(s), 20);
    free_state(s);
}

/* pawn moves */

static void test_pawn_from_start_has_two_moves(void) {
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);
    /* e-pawn: e3 and e4 */
    CHECK_TRUE(has_move(s, sq('e', 2), sq('e', 3)));
    CHECK_TRUE(has_move(s, sq('e', 2), sq('e', 4)));
    CHECK_EQ(count_legal_moves_from(s, sq('e', 2)), 2);
    free_state(s);
}

static void test_pawn_blocked_has_no_moves(void) {
    /* White pawn on e4, black pawn directly in front on e5 - no pawn moves */
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/8/4p3/4P3/8/8/4K3 w - - 0 1");
    CHECK_EQ(count_legal_moves_from(s, sq('e', 4)), 0);
    free_state(s);
}

static void test_pawn_not_on_start_row_has_one_move(void) {
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/8/8/8/4P3/8/4K3 w - - 0 1");
    CHECK_EQ(count_legal_moves_from(s, sq('e', 3)), 1);
    CHECK_TRUE(has_move(s, sq('e', 3), sq('e', 4)));
    free_state(s);
}

static void test_pawn_diagonal_capture(void) {
    /* White pawn on e4 can capture black pawn on d5 */
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1");
    CHECK_TRUE(has_move(s, sq('e', 4), sq('d', 5)));
    /* e4 not on starting row → single push (e5) plus capture (d5) = 2 moves */
    CHECK_EQ(count_legal_moves_from(s, sq('e', 4)), 2);
    free_state(s);
}

static void test_pawn_cannot_capture_backward(void) {
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/8/4P3/3p4/8/8/4K3 w - - 0 1");
    /* White pawn on e5 - black pawn on d4 is behind, not capturable */
    CHECK_FALSE(has_move(s, sq('e', 5), sq('d', 4)));
    free_state(s);
}

static void test_white_en_passant_generated(void) {
    /* White pawn on e5, black just pushed f7→f5 (ep target = f6) */
    state_t *s = new_state();
    load_fen_string(s,
        "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
    CHECK_TRUE(has_move(s, sq('e', 5), sq('f', 6)));
    free_state(s);
}

static void test_black_en_passant_generated(void) {
    /* Black pawn on d4, white just pushed e2→e4 (ep target = e3) */
    state_t *s = new_state();
    load_fen_string(s,
        "rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    CHECK_TRUE(has_move(s, sq('d', 4), sq('e', 3)));
    free_state(s);
}

static void test_en_passant_not_generated_without_target(void) {
    /* Same structure but no ep target in state */
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/8/4Pp2/8/8/8/4K3 w - - 0 1");
    CHECK_FALSE(has_move(s, sq('e', 5), sq('f', 6)));
    free_state(s);
}

static void test_pawn_a_file_no_left_capture_wrap(void) {
    /* White pawn on a5, black pawn on b6 - only forward and right captures */
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/1p6/P7/8/8/8/4K3 w - - 0 1");
    /* a5→h6 would be a wrap - must not exist */
    CHECK_FALSE(has_move(s, sq('a', 5), sq('h', 6)));
    /* a5→b6 is a valid capture */
    CHECK_TRUE(has_move(s, sq('a', 5), sq('b', 6)));
    free_state(s);
}

static void test_black_pawn_h_file_no_right_capture_wrap(void) {
    /* Black pawn on h4, white pawn on g3 - only forward and left captures, no wrap */
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/8/8/7p/6P1/8/4K3 b - - 0 1");
    /* h4→a3 would be a wrap - must not exist */
    CHECK_FALSE(has_move(s, sq('h', 4), sq('a', 3)));
    /* h4→g3 is a valid capture */
    CHECK_TRUE(has_move(s, sq('h', 4), sq('g', 3)));
    free_state(s);
}

static void test_pawn_promotion_generates_four_moves(void) {
    /* White pawn on e7 ready to promote - 4 promotions (Q R B N).
     * Kings must be off the e8 path; place them at corners. */
    state_t *s = new_state();
    load_fen_string(s, "7k/4P3/8/8/8/8/8/K7 w - - 0 1");
    CHECK_EQ(count_legal_moves_from(s, sq('e', 7)), 4);
    free_state(s);
}

static void test_black_pawn_starting_moves(void) {
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);
    /* Switch to black's perspective */
    state_t *after_e4 = new_state();
    load_fen_string(after_e4,
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    CHECK_TRUE(has_move(after_e4, sq('d', 7), sq('d', 6)));
    CHECK_TRUE(has_move(after_e4, sq('d', 7), sq('d', 5)));
    CHECK_EQ(count_legal_moves_from(after_e4, sq('d', 7)), 2);
    free_state(s);
    free_state(after_e4);
}

/* knights */

static void test_knight_center_eight_moves(void) {
    /* Knight on e4, no other pieces except kings */
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/8/8/4N3/8/8/4K3 w - - 0 1");
    CHECK_EQ(count_legal_moves_from(s, sq('e', 4)), 8);
    free_state(s);
}

static void test_knight_corner_two_moves(void) {
    /* Knight in corner a1 */
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/8/8/8/8/8/N3K3 w - - 0 1");
    CHECK_EQ(count_legal_moves_from(s, sq('a', 1)), 2);
    CHECK_TRUE(has_move(s, sq('a', 1), sq('b', 3)));
    CHECK_TRUE(has_move(s, sq('a', 1), sq('c', 2)));
    free_state(s);
}

static void test_knight_no_wrap_from_h_file(void) {
    /* Knight on h4 - offsets that would wrap to a-file must be excluded */
    state_t *s = new_state();
    load_fen_string(s, "4k3/8/8/8/7N/8/8/4K3 w - - 0 1");
    int n = count_legal_moves_from(s, sq('h', 4));
    /* h4 knight: valid squares are f3,f5,g2,g6 (4 squares) */
    CHECK_EQ(n, 4);
    free_state(s);
}

/* sliding pieces */

static void test_rook_open_board_14_moves(void) {
    /* Rook on e4 alone (with kings off to the side) */
    state_t *s = new_state();
    load_fen_string(s, "7k/8/8/8/4R3/8/8/7K w - - 0 1");
    CHECK_EQ(count_legal_moves_from(s, sq('e', 4)), 14);
    free_state(s);
}

static void test_rook_blocked_by_own_piece(void) {
    /* Rook on a1, own pawn on a4 - rook can reach a2,a3 but not a5+ */
    state_t *s = new_state();
    load_fen_string(s, "7k/8/8/8/P7/8/8/R6K w - - 0 1");
    CHECK_FALSE(has_move(s, sq('a', 1), sq('a', 5)));
    CHECK_TRUE (has_move(s, sq('a', 1), sq('a', 2)));
    free_state(s);
}

static void test_rook_can_capture_opponent(void) {
    state_t *s = new_state();
    load_fen_string(s, "7k/8/8/8/p7/8/8/R6K w - - 0 1");
    /* Can capture the black pawn on a4 but not go to a5 */
    CHECK_TRUE (has_move(s, sq('a', 1), sq('a', 4)));
    CHECK_FALSE(has_move(s, sq('a', 1), sq('a', 5)));
    free_state(s);
}

static void test_bishop_open_board_13_moves(void) {
    /* Bishop on e4.  Kings on a1/h8 so neither diagonal is blocked.
     * e4 diagonals: NE(f5,g6,h7)=3, NW(d5,c6,b7,a8)=4, SE(f3,g2,h1)=3, SW(d3,c2,b1)=3 → 13 */
    state_t *s = new_state();
    load_fen_string(s, "7k/8/8/8/4B3/8/8/K7 w - - 0 1");
    CHECK_EQ(count_legal_moves_from(s, sq('e', 4)), 13);
    free_state(s);
}

static void test_bishop_no_file_wrap(void) {
    /* Bishop on a1 - moving NE (diagonally) must not wrap from h→a */
    state_t *s = new_state();
    load_fen_string(s, "7k/8/8/8/8/8/8/B6K w - - 0 1");
    /* All moves should be on the a1-h8 diagonal only */
    move_collection_t *moves = get_legal_moves_of_state(s);
    bool wrap_detected = false;
    move_t *m;
    while ((m = pop_collection_head(moves)) != NULL) {
        if (get_move_from_square(m) == sq('a', 1)) {
            /* Every bishop move from a1 should stay on the a1–h8 diagonal */
            int to_idx = __builtin_ctzll(get_move_to_square(m));
            if ((to_idx % 8) != (to_idx / 8)) wrap_detected = true;
        }
        free_move(m);
    }
    free_move_collection(moves);
    CHECK_FALSE(wrap_detected);
    free_state(s);
}

/* king moves */

static void test_king_center_eight_moves(void) {
    state_t *s = new_state();
    load_fen_string(s, "7k/8/8/8/4K3/8/8/8 w - - 0 1");
    CHECK_EQ(count_legal_moves_from(s, sq('e', 4)), 8);
    free_state(s);
}

static void test_king_corner_three_moves(void) {
    state_t *s = new_state();
    load_fen_string(s, "7k/8/8/8/8/8/8/K7 w - - 0 1");
    CHECK_EQ(count_legal_moves_from(s, sq('a', 1)), 3);
    free_state(s);
}

static void test_king_cannot_move_into_check(void) {
    /* White king on e1, black rook on e8 - king cannot stay on e-file */
    state_t *s = new_state();
    load_fen_string(s, "4r2k/8/8/8/8/8/8/4K3 w - - 0 1");
    CHECK_FALSE(has_move(s, sq('e', 1), sq('e', 2)));
    free_state(s);
}

static void test_king_cannot_capture_defended_piece(void) {
    /* White king on e1 next to black rook on f1, but f1 is defended by another rook */
    state_t *s = new_state();
    load_fen_string(s, "5r1k/8/8/8/8/8/8/4Kr2 w - - 0 1");
    CHECK_FALSE(has_move(s, sq('e', 1), sq('f', 1)));
    free_state(s);
}

/* castling */

static void test_castling_available_on_clear_path(void) {
    /* Both sides have clear castling paths */
    state_t *s = new_state();
    load_fen_string(s, "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    /* White kingside: e1→g1 */
    CHECK_TRUE(has_move(s, sq('e', 1), sq('g', 1)));
    /* White queenside: e1→c1 */
    CHECK_TRUE(has_move(s, sq('e', 1), sq('c', 1)));
    free_state(s);
}

static void test_castling_not_available_when_blocked(void) {
    state_t *s = new_state();
    load_fen_string(s, STARTING_FEN);
    /* In starting position castling paths are blocked by pieces */
    CHECK_FALSE(has_move(s, sq('e', 1), sq('g', 1)));
    CHECK_FALSE(has_move(s, sq('e', 1), sq('c', 1)));
    free_state(s);
}

static void test_castling_not_available_without_rights(void) {
    state_t *s = new_state();
    load_fen_string(s, "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w - - 0 1");
    CHECK_FALSE(has_move(s, sq('e', 1), sq('g', 1)));
    CHECK_FALSE(has_move(s, sq('e', 1), sq('c', 1)));
    free_state(s);
}

/* legality - pins, checks */

static void test_pinned_piece_cannot_move_off_pin(void) {
    /* White bishop on e3 is pinned by black rook on a7 through king on e1 */
    /* Actually use simpler: rook pin along the e-file */
    state_t *s = new_state();
    /* White rook on e3 pinned by black rook on e8, king on e1 */
    load_fen_string(s, "4r2k/8/8/8/8/4R3/8/4K3 w - - 0 1");
    /* Rook can only move along the e-file (pin direction) */
    CHECK_FALSE(has_move(s, sq('e', 3), sq('a', 3)));
    CHECK_FALSE(has_move(s, sq('e', 3), sq('h', 3)));
    /* Moving along the pin is allowed */
    CHECK_TRUE(has_move(s, sq('e', 3), sq('e', 4)));
    CHECK_TRUE(has_move(s, sq('e', 3), sq('e', 8))); /* capture the attacker */
    free_state(s);
}

static void test_must_escape_check(void) {
    /* White is in check from black rook on e8; only legal moves are those that
     * resolve the check (block, capture, or king move). */
    state_t *s = new_state();
    /* King on e1, rook on e8, nothing in between - king must move */
    load_fen_string(s, "4r2k/8/8/8/8/8/8/4K3 w - - 0 1");
    move_collection_t *moves = get_legal_moves_of_state(s);
    bool any_non_king = false;
    move_t *m;
    while ((m = pop_collection_head(moves)) != NULL) {
        if (get_move_from_square(m) != sq('e', 1)) any_non_king = true;
        free_move(m);
    }
    free_move_collection(moves);
    /* No non-king move can resolve the check in this position */
    CHECK_FALSE(any_non_king);
    free_state(s);
}


int main(void) {
    magic_init();
    test_starting_pos_20_moves();
    test_pawn_from_start_has_two_moves();
    test_pawn_blocked_has_no_moves();
    test_pawn_not_on_start_row_has_one_move();
    test_pawn_diagonal_capture();
    test_pawn_cannot_capture_backward();
    test_white_en_passant_generated();
    test_black_en_passant_generated();
    test_en_passant_not_generated_without_target();
    test_pawn_a_file_no_left_capture_wrap();
    test_black_pawn_h_file_no_right_capture_wrap();
    test_pawn_promotion_generates_four_moves();
    test_black_pawn_starting_moves();
    test_knight_center_eight_moves();
    test_knight_corner_two_moves();
    test_knight_no_wrap_from_h_file();
    test_rook_open_board_14_moves();
    test_rook_blocked_by_own_piece();
    test_rook_can_capture_opponent();
    test_bishop_open_board_13_moves();
    test_bishop_no_file_wrap();
    test_king_center_eight_moves();
    test_king_corner_three_moves();
    test_king_cannot_move_into_check();
    test_king_cannot_capture_defended_piece();
    test_castling_available_on_clear_path();
    test_castling_not_available_when_blocked();
    test_castling_not_available_without_rights();
    test_pinned_piece_cannot_move_off_pin();
    test_must_escape_check();
    RESULT();
}
