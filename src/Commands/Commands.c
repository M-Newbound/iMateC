/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "Commands.h"
#include <stdio.h>

// Forward declare command functions
void position_command   (const CommandParams params);
void go_command         (const CommandParams params);
void print_command      (const CommandParams params);
void quit_command       (const CommandParams params);
void help_command       (const CommandParams params);
void move_command       (const CommandParams params);
void status_command     (const CommandParams params);

/**
 * @brief Array of all engine commands.
 *
 * This array contains all the commands that the engine can handle. Each command is represented by a Command struct,
 * which contains a function pointer to the command's implementation and a regex string that matches the command's syntax.
 */
const Command ENGINE_COMMANDS[] = {
    {help_command,          "^help$"},
    {position_command,      "^position((startpos)|())$"},
    {go_command,            "^go$"},
    {print_command,         "^print((board)|(moves([a-f][1-8])))$"},
    {quit_command,          "^quit$"},
    {move_command,          "^move$"},
    {status_command,        "^status$"}
};

/**
 * @brief Returns the number of commands in the ENGINE_COMMANDS array.
 *
 * This function is used to get the number of commands that the engine can handle.
 *
 * @return The number of commands in the ENGINE_COMMANDS array.
 */
int length_of_engine_commands() {
    return sizeof(ENGINE_COMMANDS) / sizeof(ENGINE_COMMANDS[0]);
}