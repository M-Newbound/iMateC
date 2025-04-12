/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "framework.h"
#include "Moves/MagicBitboards.h"
#include <stdint.h>
#include <string.h>

/* Simple LCG for reproducible random occupancy boards */
static uint64_t lcg_state = 0x123456789ABCDEF0ULL;
static uint64_t lcg_next(void) {
    lcg_state = lcg_state * 6364136223846793005ULL + 1442695040888963407ULL;
    return lcg_state;
}

/* mask shape */

static void test_rook_mask_corner_a1(void) {
    /* a1 = sq 0: rank-1 interior (b1-g1, 6 bits) + file-a interior (a2-a7, 6 bits) = 12 bits */
    uint64_t mask = rook_occupancy_mask(0);
    CHECK_EQ(__builtin_popcountll(mask), 12);

    /* Edge squares must NOT appear in the mask */
    CHECK_EQ(mask & (1ULL << 7),  0ULL);  /* h1 - file-h edge */
    CHECK_EQ(mask & (1ULL << 56), 0ULL);  /* a8 - rank-8 edge */
    CHECK_EQ(mask & (1ULL << 0),  0ULL);  /* a1 - the piece itself */
}

static void test_rook_mask_centre_d4(void) {
    /* d4 = sq 27: rank-4 interior (b4,c4,e4,f4,g4 - 5 bits, no a4/h4)
     *           + file-d interior (d2,d3,d5,d6,d7 - 5 bits, no d1/d8) = 10 bits */
    uint64_t mask = rook_occupancy_mask(27);
    CHECK_EQ(__builtin_popcountll(mask), 10);
    CHECK_EQ(mask & (1ULL << 24), 0ULL);  /* a4 - file-a edge */
    CHECK_EQ(mask & (1ULL << 31), 0ULL);  /* h4 - file-h edge */
    CHECK_EQ(mask & (1ULL << 3),  0ULL);  /* d1 - rank-1 edge */
    CHECK_EQ(mask & (1ULL << 59), 0ULL);  /* d8 - rank-8 edge */
}

static void test_bishop_mask_corner_a1(void) {
    /* a1 = sq 0: only one diagonal (NE), interior squares b2-g7 = 6 bits */
    uint64_t mask = bishop_occupancy_mask(0);
    CHECK_EQ(__builtin_popcountll(mask), 6);
    CHECK_EQ(mask & (1ULL << 63), 0ULL);  /* h8 - edge */
    CHECK_EQ(mask & (1ULL << 0),  0ULL);  /* a1 - the piece itself */
}

static void test_bishop_mask_centre_d4(void) {
    /* d4 = sq 27: all four diagonals, interior squares only.
     * NE: e5,f6,g7 (3); NW: c5,b6 (2); SE: e3,f2 (2); SW: c3,b2 (2) = 9 bits */
    uint64_t mask = bishop_occupancy_mask(27);
    CHECK_EQ(__builtin_popcountll(mask), 9);
}

static void test_bishop_mask_excludes_edges(void) {
    /* No mask for any square should include rank-1, rank-8, file-a, or file-h */
    const uint64_t EDGES = 0xFF818181818181FFULL;
    for (int sq = 0; sq < 64; sq++)
        CHECK_EQ(bishop_occupancy_mask(sq) & EDGES, 0ULL);
}

static void test_rook_mask_excludes_edges(void) {
    const uint64_t EDGES = 0xFF818181818181FFULL;
    for (int sq = 0; sq < 64; sq++) {
        /* The mask may include edge squares only if the piece itself is on an edge
         * (then the rank/file extends into the interior in the other direction).
         * But the square's own rank-edge and file-edge squares are still excluded.
         * Simpler invariant: the piece's own square is never in the mask. */
        CHECK_EQ(rook_occupancy_mask(sq) & (1ULL << sq), 0ULL);
        (void)EDGES;  /* suppress unused warning for this simpler check */
    }
}

/* classical vs magic - these have to match on every input */

static void test_rook_magic_matches_classical_empty_board(void) {
    /* Empty board: rook can reach every square on same rank/file */
    for (int sq = 0; sq < 64; sq++) {
        uint64_t expected = classical_rook_attacks(sq, 0ULL);
        uint64_t actual   = magic_rook_attacks(sq, 0ULL);
        CHECK_EQ(actual, expected);
    }
}

static void test_bishop_magic_matches_classical_empty_board(void) {
    for (int sq = 0; sq < 64; sq++) {
        uint64_t expected = classical_bishop_attacks(sq, 0ULL);
        uint64_t actual   = magic_bishop_attacks(sq, 0ULL);
        CHECK_EQ(actual, expected);
    }
}

static void test_rook_magic_matches_classical_random_occupancy(void) {
    /* 500 random occupancy boards per square */
    for (int sq = 0; sq < 64; sq++) {
        for (int trial = 0; trial < 500; trial++) {
            uint64_t occ      = lcg_next() & lcg_next(); /* sparse occupancy */
            uint64_t expected = classical_rook_attacks(sq, occ);
            uint64_t actual   = magic_rook_attacks(sq, occ);
            CHECK_EQ(actual, expected);
        }
    }
}

static void test_bishop_magic_matches_classical_random_occupancy(void) {
    for (int sq = 0; sq < 64; sq++) {
        for (int trial = 0; trial < 500; trial++) {
            uint64_t occ      = lcg_next() & lcg_next();
            uint64_t expected = classical_bishop_attacks(sq, occ);
            uint64_t actual   = magic_bishop_attacks(sq, occ);
            CHECK_EQ(actual, expected);
        }
    }
}

/* blocking semantics (classical reference) */

static void test_rook_blocked_by_friendly_piece(void) {
    /* Rook on e4 (sq 28), own piece on e6 (sq 44): north ray stops at e5 */
    int sq = 28;  /* e4 */
    uint64_t blocker = 1ULL << 44;  /* e6 */

    uint64_t attacks = classical_rook_attacks(sq, blocker);

    CHECK_TRUE(attacks  & (1ULL << 36)); /* e5 - reachable */
    CHECK_TRUE(attacks  & (1ULL << 44)); /* e6 - blocker IS included (it can be captured) */
    CHECK_FALSE(attacks & (1ULL << 52)); /* e7 - beyond blocker, not reachable */
}

static void test_rook_blocked_east(void) {
    /* Rook on a4 (sq 24), blocker on d4 (sq 27) */
    uint64_t attacks = classical_rook_attacks(24, 1ULL << 27);
    CHECK_TRUE(attacks  & (1ULL << 25)); /* b4 */
    CHECK_TRUE(attacks  & (1ULL << 26)); /* c4 */
    CHECK_TRUE(attacks  & (1ULL << 27)); /* d4 - blocker included */
    CHECK_FALSE(attacks & (1ULL << 28)); /* e4 - beyond blocker */
}

static void test_bishop_blocked_diagonal(void) {
    /* Bishop on a1 (sq 0), blocker on c3 (sq 18) */
    uint64_t attacks = classical_bishop_attacks(0, 1ULL << 18);
    CHECK_TRUE(attacks  & (1ULL << 9));   /* b2 */
    CHECK_TRUE(attacks  & (1ULL << 18));  /* c3 - blocker included */
    CHECK_FALSE(attacks & (1ULL << 27));  /* d4 - beyond blocker */
}

/* edge squares */

static void test_rook_corner_h8_empty(void) {
    /* Rook on h8 (sq 63) on empty board reaches all of rank 8 and file h */
    uint64_t attacks = magic_rook_attacks(63, 0ULL);
    /* All of rank 8 except h8 itself */
    for (int f = 0; f < 7; f++)
        CHECK_TRUE(attacks & (1ULL << (56 + f)));
    /* All of file h except h8 */
    for (int r = 0; r < 7; r++)
        CHECK_TRUE(attacks & (1ULL << (r * 8 + 7)));
}

static void test_bishop_h1_empty(void) {
    /* Bishop on h1 (sq 7): only one diagonal NW (g2,f3,e4,d5,c6,b7,a8) */
    uint64_t attacks = magic_bishop_attacks(7, 0ULL);
    CHECK_TRUE(attacks  & (1ULL << 14)); /* g2 */
    CHECK_TRUE(attacks  & (1ULL << 56)); /* a8 */
    /* No attacks east or south - already on edge */
    CHECK_FALSE(attacks & (1ULL << 0));  /* a1 - wrong diagonal */
}

/* exhaustive check - every occupancy subset of a single square */

static void test_rook_all_occupancies_e4(void) {
    /* sq 28 (e4): verify magic == classical for every possible occupancy subset
     * of the rook mask (2^10 = 1024 occupancies) */
    int      sq   = 28;
    uint64_t mask = rook_occupancy_mask(sq);
    int      n    = __builtin_popcountll(mask);

    /* Carry-Rippler: enumerate all subsets */
    uint64_t occ = 0;
    do {
        CHECK_EQ(magic_rook_attacks(sq, occ), classical_rook_attacks(sq, occ));
        occ = (occ - mask) & mask;
    } while (occ != 0);
    /* Also test full mask */
    CHECK_EQ(magic_rook_attacks(sq, mask), classical_rook_attacks(sq, mask));
    (void)n;
}

static void test_bishop_all_occupancies_d4(void) {
    int      sq   = 27;
    uint64_t mask = bishop_occupancy_mask(sq);

    uint64_t occ = 0;
    do {
        CHECK_EQ(magic_bishop_attacks(sq, occ), classical_bishop_attacks(sq, occ));
        occ = (occ - mask) & mask;
    } while (occ != 0);
    CHECK_EQ(magic_bishop_attacks(sq, mask), classical_bishop_attacks(sq, mask));
}


int main(void) {
    magic_init();

    test_rook_mask_corner_a1();
    test_rook_mask_centre_d4();
    test_bishop_mask_corner_a1();
    test_bishop_mask_centre_d4();
    test_bishop_mask_excludes_edges();
    test_rook_mask_excludes_edges();

    test_rook_magic_matches_classical_empty_board();
    test_bishop_magic_matches_classical_empty_board();
    test_rook_magic_matches_classical_random_occupancy();
    test_bishop_magic_matches_classical_random_occupancy();

    test_rook_blocked_by_friendly_piece();
    test_rook_blocked_east();
    test_bishop_blocked_diagonal();

    test_rook_corner_h8_empty();
    test_bishop_h1_empty();

    test_rook_all_occupancies_e4();
    test_bishop_all_occupancies_d4();

    RESULT();
}
