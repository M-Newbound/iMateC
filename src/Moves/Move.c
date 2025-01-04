/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "Move.h"
#include <stdlib.h>
#include "../State/GameState.h"

struct move {
    uint64_t from_square;
    uint64_t to_square;
    flags_t  flags;
};

move_t *new_move(uint64_t from_square, uint64_t to_square, flags_t flags) {
    move_t *move = malloc(sizeof(move_t));
    move->from_square = from_square;
    move->to_square   = to_square;
    move->flags       = flags;
    return move;
}

void free_move(move_t *move) {
    free(move);
}

const flags_t *get_move_flags(const move_t *move) {
    return &move->flags;
}

uint64_t get_move_from_square(const move_t *move) {
    return move->from_square;
}

uint64_t get_move_to_square(const move_t *move) {
    return move->to_square;
}
