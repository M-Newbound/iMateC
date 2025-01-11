/* iMate -- Copyright (C) 2024 Martin Newbound */

#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../State/GameState.h"
#include <regex.h>
#include <stdbool.h>

#define MAX_MATCHES 10

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

/* Parameters passed to every command handler. raw_input preserves the
 * original line (spaces intact) while user_input has whitespace stripped
 * for regex matching. matches[] holds captured groups from the regex. */
typedef struct {
    char    *raw_input;
    char    *user_input;
    bool    *engine_is_running;
    state_t *engine_game_state;
    char    *matches[MAX_MATCHES];
} CommandParams;

typedef void (*CommandFunc)(const CommandParams params);

typedef struct {
    const CommandFunc func;
    const char       *regex;
} Command;

extern const Command ENGINE_COMMANDS[];

int length_of_engine_commands(void);

#ifdef __cplusplus
}
#endif

#endif /* COMMANDS_H */
