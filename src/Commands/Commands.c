/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "Commands.h"
#include <stdio.h>

// UCI core
void uci_command(const CommandParams params);
void isready_command(const CommandParams params);
void ucinewgame_command(const CommandParams params);
void stop_command(const CommandParams params);

// game
void position_command(const CommandParams params);
void go_command(const CommandParams params);

// output
void print_command(const CommandParams params);
void status_command(const CommandParams params);

// interactive
void move_command(const CommandParams params);
void help_command(const CommandParams params);
void quit_command(const CommandParams params);

// whitespace is stripped from user_input before matching so "go depth 6" becomes "godepth6".
// commands that need their arguments (position, go) use raw_input instead.
// UCI commands listed first so they take priority.
const Command ENGINE_COMMANDS[] = {
    {uci_command,        "^uci$"},
    {isready_command,    "^isready$"},
    {ucinewgame_command, "^ucinewgame$"},
    {stop_command,       "^stop$"},

    {position_command,   "^position"},
    {go_command,         "^go"},

    {print_command,      "^print((board)|(moves([a-h][1-8])))$"},
    {status_command,     "^status$"},
    {move_command,       "^move[a-h][1-8][a-h][1-8][qrbn]?$"},
    {help_command,       "^help$"},
    {quit_command,       "^quit$"},
};

int length_of_engine_commands(void) {
    return (int)(sizeof(ENGINE_COMMANDS) / sizeof(ENGINE_COMMANDS[0]));
}
