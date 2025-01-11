/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "IMate.h"
#include "State/GameState.h"
#include "Commands/Commands.h"
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

// compile once at startup
#define MAX_COMMANDS 16
static regex_t compiled_regexes[MAX_COMMANDS];

static void compile_regexes(int count) {
    for (int i = 0; i < count; i++) {
        if (regcomp(&compiled_regexes[i], ENGINE_COMMANDS[i].regex, REG_EXTENDED) != 0) {
            fprintf(stderr, "fatal: failed to compile command regex: %s\n",
                    ENGINE_COMMANDS[i].regex);
            exit(EXIT_FAILURE);
        }
    }
}

static void free_compiled_regexes(int count) {
    for (int i = 0; i < count; i++) regfree(&compiled_regexes[i]);
}

// remove whitespace so "print board" becomes "printboard" for regex matching
static void remove_whitespace(char *s) {
    char *rd = s, *wr = s;
    while (*rd) {
        if (!isspace((unsigned char)*rd)) *wr++ = *rd;
        rd++;
    }
    *wr = '\0';
}

void engine_loop(void) {
    // grown by getline() as needed -- long UCI "position ... moves ..." lines
    // (a full game's move list, resent on every move) must never be truncated
    char *raw_input = NULL;
    size_t raw_cap = 0;

    printf("Tip: Type \"help\" to see a list of commands\n");

    state_t *game_state = new_state();
    bool is_running = true;

    int num_commands = length_of_engine_commands();
    compile_regexes(num_commands);

    while (is_running) {
        if (getline(&raw_input, &raw_cap, stdin) < 0) break;

        // raw_input keeps whitespace intact - position/go need it for FEN parsing
        char *user_input = strdup(raw_input);
        remove_whitespace(user_input);
        if (user_input[0] == '\0') {
            free(user_input);
            continue;
        }

        bool matched = false;
        for (int i = 0; i < num_commands && !matched; i++) {
            regmatch_t rm[MAX_MATCHES];
            memset(rm, 0, sizeof(rm));

            if (regexec(&compiled_regexes[i], user_input, MAX_MATCHES, rm, 0) != 0)
                continue;

            CommandParams params = {
                .raw_input         = raw_input,
                .user_input        = user_input,
                .engine_is_running = &is_running,
                .engine_game_state = game_state
            };
            memset(params.matches, 0, sizeof(params.matches));

            for (int j = 0; j < MAX_MATCHES; j++) {
                if (rm[j].rm_so == -1) continue;
                int len = (int)(rm[j].rm_eo - rm[j].rm_so);
                params.matches[j] = strndup(user_input + rm[j].rm_so, len);
            }

            ENGINE_COMMANDS[i].func(params);
            printf("\n");

            for (int j = 0; j < MAX_MATCHES; j++) {
                free(params.matches[j]);
                params.matches[j] = NULL;
            }

            matched = true;
        }

        if (!matched)
            printf("Unknown command. Type \"help\" for a list of commands.\n");

        free(user_input);
    }

    free(raw_input);
    free_compiled_regexes(num_commands);
    free_state(game_state);
}
