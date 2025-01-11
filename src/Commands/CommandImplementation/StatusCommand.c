/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include <stdio.h>

void status_command(const CommandParams params) {
    switch (get_state_status(params.engine_game_state)) {
        case WHITE_CHECKMATE: printf("White is in checkmate.\n"); return;
        case BLACK_CHECKMATE: printf("Black is in checkmate.\n"); return;
        case STALEMATE:       printf("Stalemate.\n");             return;
        case WHITE_CHECK:     printf("White is in check.\n");     return;
        case BLACK_CHECK:     printf("Black is in check.\n");     return;
        default: break;
    }
    color_t c = side_to_move(params.engine_game_state);
    printf("%s to move.\n", c == WHITE ? "White" : "Black");
}
