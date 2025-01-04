/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../MoveGeneration.h"
#include "../MagicBitboards.h"

void gen_bishop_moves_on_square(const state_t *state, move_collection_t *collection,
                                 uint64_t sq_bb) {
    const color_t color_to_move = side_to_move(state);
    const uint64_t own_bb = color_bb(state, color_to_move);
    const uint64_t opp_bb = color_bb(state, (color_to_move == WHITE) ? BLACK : WHITE);
    const uint64_t occupied = own_bb | opp_bb;

    const flags_t flags = make_quiet_flags();

    // magic lookup -- includes first blocker on each diagonal, excludes own pieces
    uint64_t attacks = magic_bishop_attacks(__builtin_ctzll(sq_bb), occupied) & ~own_bb;

    while (attacks) {
        uint64_t to = attacks & (uint64_t)(-(int64_t)attacks); // isolate LSB
        push_move_to_collection(new_move(sq_bb, to, flags), collection);
        attacks &= attacks - 1; // clear LSB
    }
}
