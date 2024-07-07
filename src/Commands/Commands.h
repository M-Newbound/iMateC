/**
 * @file Commands.h
 * @brief This file contains the declarations of the structures and functions used for handling commands in the chess engine.
 * 
 * @details The CommandParams structure holds the parameters for a command. The CommandFunc type is a function pointer type for a command function.
 * The Command structure represents a command. The ENGINE_COMMANDS array contains all the commands that the engine can handle.
 * The length_of_engine_commands function returns the number of commands in the ENGINE_COMMANDS array.
 * 
 * @version 1.0.0
 * @author Martin Newbound
 * @date 2024
 * 
 * @note License:
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */


#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../State/GameState.h"
#include <regex.h>
#include <stdbool.h>

#define MAX_MATCHES 10

/**
 * @struct CommandParams
 * @brief A structure to hold the parameters for a command.
 *
 * This structure contains the user input, a pointer to the engine's running status, a pointer to the engine's game state,
 * and an array of matches for the command's regex.
 */
typedef struct {
    char *user_input;
    bool *engine_is_running;
    state_t *engine_game_state;
    char *matches[MAX_MATCHES];
} CommandParams;

/**
 * @typedef CommandFunc
 * @brief A function pointer type for a command function.
 *
 * This type is used for function pointers to command functions. A command function takes a CommandParams structure as its only parameter.
 */
typedef void (*CommandFunc)(const CommandParams params);

/**
 * @struct Command
 * @brief A structure to represent a command.
 *
 * This structure contains a function pointer to the command's implementation and a regex string that matches the command's syntax.
 */
typedef struct {
    const CommandFunc func;
    const char* regex;
} Command;

extern const Command ENGINE_COMMANDS[];

/**
 * @brief Returns the number of commands in the ENGINE_COMMANDS array.
 *
 * This function is used to get the number of commands that the engine can handle.
 *
 * @return The number of commands in the ENGINE_COMMANDS array.
 */
int length_of_engine_commands();

#ifdef __cplusplus
}
#endif
#endif // COMMANDS_H