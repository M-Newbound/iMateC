/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include <stdio.h>
#include "../../State/GameState.h"

/**
 * @brief Executes the 'status' command.
 *
 * This function is responsible for checking the status of the game and printing it.
 * It checks if the game is in stalemate, checkmate, or check for both white and black.
 * 
 * @param params The command parameters, including the current game state.
 */
void status_command(const CommandParams params) {
    typedef bool (*check_func_t)(const state_t *, color_t);
    check_func_t check_funcs[] = {is_stalemate, is_checkmate, is_check};
    const char *messages[] = {" is in stalemate.", " is in checkmate.", " is in check."};
    color_t colors[] = {WHITE, BLACK};

    for (int color = 0; color < 2; color++) {
        for (int i = 0; i < sizeof(check_funcs) / sizeof(check_funcs[0]); i++) {
            if (check_funcs[i](params.engine_game_state, colors[color])) {
                printf("%s%s\n", (colors[color] == WHITE) ? "White" : "Black", messages[i]);
                return;
            }
        }
    }
}