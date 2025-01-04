/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../MoveGeneration.h"

void gen_rook_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t sq_bb);
void gen_bishop_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t sq_bb);

// queen combines rook and bishop movement
void gen_queen_moves_on_square(const state_t *state, move_collection_t *collection,
                                uint64_t sq_bb) {
    gen_rook_moves_on_square(state, collection, sq_bb);
    gen_bishop_moves_on_square(state, collection, sq_bb);
}
