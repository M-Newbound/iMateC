/* iMate -- Copyright (C) 2024 Martin Newbound */

/*
 * Magic bitboard attack lookups for sliding pieces.
 *
 * The idea: pre-compute an attack table for every (square, occupancy) pair,
 * then use a magic multiplier to hash any occupancy bitboard down to a table
 * index in O(1):
 *
 *   index   = (occupied & mask[sq]) * magic[sq] >> (64 - bits[sq])
 *   attacks = table[sq][index]
 *
 * Call magic_init() once at startup. Its safe to call it more than once,
 * subsequent calls are no-ops.
 */

#ifndef MAGIC_BITBOARDS_H
#define MAGIC_BITBOARDS_H

#include <stdint.h>

void     magic_init(void);

uint64_t magic_rook_attacks  (int sq, uint64_t occupied);
uint64_t magic_bishop_attacks(int sq, uint64_t occupied);

/* Exposed for the test suite - not needed outside of tests/MagicBitboards.c */
uint64_t rook_occupancy_mask    (int sq);
uint64_t bishop_occupancy_mask  (int sq);
uint64_t classical_rook_attacks  (int sq, uint64_t occ);
uint64_t classical_bishop_attacks(int sq, uint64_t occ);

#endif /* MAGIC_BITBOARDS_H */
