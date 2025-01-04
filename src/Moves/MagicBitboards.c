/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "MagicBitboards.h"
#include <string.h>

// XorShift64 with a fixed seed so magic numbers are the same every run
// sparse_random() ANDs three outputs together to get values with few set bits,
// which are the best candidates for magic multipliers
static uint64_t prng_state = 0xD4E12C77B5A72A3DULL;

static uint64_t prng(void) {
    prng_state ^= prng_state >> 12;
    prng_state ^= prng_state << 25;
    prng_state ^= prng_state >> 27;
    return prng_state * 0x2545F4914F6CDD1DULL;
}

static uint64_t sparse_random(void) {
    return prng() & prng() & prng();
}

// occupancy masks - edge squares are excluded because a piece on the board edge
// is always reachable regardless of what sits beyond it, so including them
// would waste index bits

static uint64_t compute_rook_mask(int sq) {
    int r = sq / 8, f = sq % 8;
    uint64_t mask = 0;

    // interior files along the rank, skip the rook's own file
    for (int ff = 1; ff <= 6; ff++)
        if (ff != f) mask |= 1ULL << (r * 8 + ff);

    // interior ranks along the file, skip the rook's own rank
    for (int rr = 1; rr <= 6; rr++)
        if (rr != r) mask |= 1ULL << (rr * 8 + f);

    return mask;
}

static uint64_t compute_bishop_mask(int sq) {
    int r = sq / 8, f = sq % 8;
    uint64_t mask = 0;

    // all four diagonals, stopping one square before the board edge
    for (int rr = r + 1, ff = f + 1; rr <= 6 && ff <= 6; rr++, ff++) mask |= 1ULL << (rr * 8 + ff);
    for (int rr = r + 1, ff = f - 1; rr <= 6 && ff >= 1; rr++, ff--) mask |= 1ULL << (rr * 8 + ff);
    for (int rr = r - 1, ff = f + 1; rr >= 1 && ff <= 6; rr--, ff++) mask |= 1ULL << (rr * 8 + ff);
    for (int rr = r - 1, ff = f - 1; rr >= 1 && ff >= 1; rr--, ff--) mask |= 1ULL << (rr * 8 + ff);

    return mask;
}

// classical (reference) attack generators -- used to fill magic tables during init
// and as ground-truth in the test suite. not called in the hot path.

uint64_t classical_rook_attacks(int sq, uint64_t occ) {
    int r = sq / 8, f = sq % 8;
    uint64_t attacks = 0;

    for (int rr = r + 1; rr <= 7; rr++) { attacks |= 1ULL << (rr * 8 + f); if (occ & (1ULL << (rr * 8 + f))) break; }
    for (int rr = r - 1; rr >= 0; rr--) { attacks |= 1ULL << (rr * 8 + f); if (occ & (1ULL << (rr * 8 + f))) break; }
    for (int ff = f + 1; ff <= 7; ff++) { attacks |= 1ULL << (r * 8 + ff);  if (occ & (1ULL << (r * 8 + ff)))  break; }
    for (int ff = f - 1; ff >= 0; ff--) { attacks |= 1ULL << (r * 8 + ff);  if (occ & (1ULL << (r * 8 + ff)))  break; }

    return attacks;
}

uint64_t classical_bishop_attacks(int sq, uint64_t occ) {
    int r = sq / 8, f = sq % 8;
    uint64_t attacks = 0;

    for (int rr=r+1, ff=f+1; rr<=7 && ff<=7; rr++, ff++) { attacks |= 1ULL<<(rr*8+ff); if (occ & (1ULL<<(rr*8+ff))) break; }
    for (int rr=r+1, ff=f-1; rr<=7 && ff>=0; rr++, ff--) { attacks |= 1ULL<<(rr*8+ff); if (occ & (1ULL<<(rr*8+ff))) break; }
    for (int rr=r-1, ff=f+1; rr>=0 && ff<=7; rr--, ff++) { attacks |= 1ULL<<(rr*8+ff); if (occ & (1ULL<<(rr*8+ff))) break; }
    for (int rr=r-1, ff=f-1; rr>=0 && ff>=0; rr--, ff--) { attacks |= 1ULL<<(rr*8+ff); if (occ & (1ULL<<(rr*8+ff))) break; }

    return attacks;
}

// carry-rippler subset enumeration
// maps an index in [0, 2^n_bits) to one subset of mask, in a stable order
static uint64_t index_to_occupancy(int index, int n_bits, uint64_t mask) {
    uint64_t occ = 0;
    uint64_t m = mask;
    for (int i = 0; i < n_bits; i++) {
        int bit = __builtin_ctzll(m);
        m &= m - 1;
        if (index & (1 << i)) occ |= 1ULL << bit;
    }
    return occ;
}

// search for a magic number for the given square
// tries sparse candidates until one maps every occupancy subset to a unique slot
// (constructive collisions are fine as long as the attack set is the same)
static uint64_t find_magic(int sq, int is_rook) {
    uint64_t mask = is_rook ? compute_rook_mask(sq) : compute_bishop_mask(sq);
    int n_bits = __builtin_popcountll(mask);
    int n_subsets = 1 << n_bits;

    uint64_t occupancies[4096];
    uint64_t attacks[4096];
    for (int i = 0; i < n_subsets; i++) {
        occupancies[i] = index_to_occupancy(i, n_bits, mask);
        attacks[i] = is_rook
            ? classical_rook_attacks(sq, occupancies[i])
            : classical_bishop_attacks(sq, occupancies[i]);
    }

    uint64_t used[4096];
    for (;;) {
        uint64_t magic = sparse_random();

        if (__builtin_popcountll((mask * magic) >> 56) < 6) continue;

        memset(used, 0, sizeof(uint64_t) * (size_t)n_subsets);

        int valid = 1;
        for (int i = 0; i < n_subsets && valid; i++) {
            int idx = (int)((occupancies[i] * magic) >> (64 - n_bits));
            if (used[idx] == 0)
                used[idx] = attacks[i];
            else if (used[idx] != attacks[i])
                valid = 0;
        }
        if (valid) return magic;
    }
}

// attack tables
// rook:   up to 12 relevant bits (corner squares) -> 4096 entries, 2 MB total
// bishop: up to 9 relevant bits (centre squares)  ->  512 entries, 256 KB total
static uint64_t rook_attack_table[64][4096];
static uint64_t bishop_attack_table[64][512];

static uint64_t rook_magics[64];
static uint64_t rook_masks[64];
static int rook_shifts[64];

static uint64_t bishop_magics[64];
static uint64_t bishop_masks[64];
static int bishop_shifts[64];

static int magic_ready = 0;

void magic_init(void) {
    if (magic_ready) return;

    for (int sq = 0; sq < 64; sq++) {

        // rook
        rook_masks[sq] = compute_rook_mask(sq);
        int rook_bits = __builtin_popcountll(rook_masks[sq]);
        rook_shifts[sq] = 64 - rook_bits;
        rook_magics[sq] = find_magic(sq, 1);

        for (int i = 0; i < (1 << rook_bits); i++) {
            uint64_t occ = index_to_occupancy(i, rook_bits, rook_masks[sq]);
            int idx = (int)((occ * rook_magics[sq]) >> rook_shifts[sq]);
            rook_attack_table[sq][idx] = classical_rook_attacks(sq, occ);
        }

        // bishop
        bishop_masks[sq] = compute_bishop_mask(sq);
        int bishop_bits = __builtin_popcountll(bishop_masks[sq]);
        bishop_shifts[sq] = 64 - bishop_bits;
        bishop_magics[sq] = find_magic(sq, 0);

        for (int i = 0; i < (1 << bishop_bits); i++) {
            uint64_t occ = index_to_occupancy(i, bishop_bits, bishop_masks[sq]);
            int idx = (int)((occ * bishop_magics[sq]) >> bishop_shifts[sq]);
            bishop_attack_table[sq][idx] = classical_bishop_attacks(sq, occ);
        }
    }

    magic_ready = 1;
}

uint64_t magic_rook_attacks(int sq, uint64_t occupied) {
    uint64_t occ = occupied & rook_masks[sq];
    int idx = (int)((occ * rook_magics[sq]) >> rook_shifts[sq]);
    return rook_attack_table[sq][idx];
}

uint64_t magic_bishop_attacks(int sq, uint64_t occupied) {
    uint64_t occ = occupied & bishop_masks[sq];
    int idx = (int)((occ * bishop_magics[sq]) >> bishop_shifts[sq]);
    return bishop_attack_table[sq][idx];
}

uint64_t rook_occupancy_mask(int sq)   { return rook_masks[sq]; }
uint64_t bishop_occupancy_mask(int sq) { return bishop_masks[sq]; }
