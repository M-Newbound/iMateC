/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include "../../Util/MoveParser.h"
#include <stdio.h>

void move_command(const CommandParams params) {
    const char *mv = params.user_input + 4; // skip "move"

    if (!apply_uci_move(params.engine_game_state, mv)) {
        printf("Illegal move: %s\n", mv);
        return;
    }
    printf("Move applied.\n");
}
