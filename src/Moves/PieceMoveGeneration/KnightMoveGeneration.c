/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../MoveGeneration.h"
#include <stdlib.h>

void gen_knight_moves_on_square(const state_t *state, move_collection_t *collection,
                                 uint64_t sq_bb) {
    const color_t color_to_move = side_to_move(state);
    const uint64_t own_bb = color_bb(state, color_to_move);

    int sq = __builtin_ctzll(sq_bb);
    int sq_file = sq % 8;

    flags_t flags = make_quiet_flags();

    // knight offsets in linear-index space
    const int offsets[] = {-17, -15, -10, -6, 6, 10, 15, 17};
    const int num_offsets = (int)(sizeof(offsets) / sizeof(offsets[0]));

    for (int i = 0; i < num_offsets; i++) {
        int to_sq = sq + offsets[i];
        if (to_sq < 0 || to_sq >= 64) continue;

        // reject wrap-arounds (knight moves at most 2 files)
        if (abs((to_sq % 8) - sq_file) > 2) continue;

        uint64_t to_bb = 1ULL << to_sq;
        if (own_bb & to_bb) continue;

        push_move_to_collection(new_move(sq_bb, to_bb, flags), collection);
    }
}
