/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include <stdio.h>

/**
 * @brief Executes the 'quit' command.
 *
 * This function is responsible for stopping the engine. It sets the 'engine_is_running' flag to false.
 * 
 * @param params The command parameters, including the current game state and the 'engine_is_running' flag.
 */
void quit_command(const CommandParams params) {
    printf("Quitting...\n");
    *params.engine_is_running = false;
}