/* iMate -- Copyright (C) 2024 Martin Newbound */                                                    

#include "Move.h"
#include <stdlib.h>
#include "../State/GameState.h"
#include "../Moves/MoveGeneration.h"
#include "../Moves/MoveCollection.h"

struct move {
    uint64_t from_square;
    uint64_t to_square;
    flags_t flags;
};

move_t *new_move(uint64_t from_square, uint64_t to_square, flags_t flags) {
    move_t *move = malloc(sizeof(move_t));
    move->from_square = from_square;
    move->to_square = to_square;

    move->flags = flags;
    return move;
}

void free_move(move_t *move)
{
    free(move);
    move = NULL;
}

bool is_legal_move(const state_t *state, const move_t *move) {
    state_t *temporary_state = new_state();
    copy_state(state, temporary_state);
    apply_move(move, temporary_state);

    game_status_t status = get_state_status(temporary_state);

    bool is_legal = true;
    if (get_state_to_move_color(state) == WHITE && (status == WHITE_CHECK || status == WHITE_CHECKMATE)) is_legal = false;
    if (get_state_to_move_color(state) == BLACK && (status == BLACK_CHECK || status == BLACK_CHECKMATE)) is_legal = false;

    free_state(temporary_state);
    return is_legal;
}



