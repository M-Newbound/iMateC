#include "IMate.h"
#include "State/GameState.h"
#include "Commands/Commands.h"
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define INPUT_BUFFER 100

typedef struct {
    state_t* game_state;
    bool is_running;
} EngineState;


void remove_whitespace(char* input) {
    char const* read_ptr = input;
    char* write_ptr = input;
    while (*read_ptr) {
        if (!isspace(*read_ptr))
            *write_ptr++ = *read_ptr;
        read_ptr++;
    }

    *write_ptr = '\0';
}


void engine_loop() {
    regex_t regex;
    char user_input[INPUT_BUFFER];
    regmatch_t matches[MAX_MATCHES];

    printf("Tip: Type \"help\" to see a list of commands \n");

    EngineState engine_state = {
        .game_state = new_state(),
        .is_running = true
    };

    while (engine_state.is_running) {
        fgets(user_input, sizeof(user_input), stdin);
        remove_whitespace(user_input);

        for (size_t i = 0; i < length_of_engine_commands(); i++) {
            regcomp(&regex, ENGINE_COMMANDS[i].regex, REG_EXTENDED);

            if (!regexec(&regex, user_input, 1, matches, 0)) {
                CommandParams cmd_params = {
                    .engine_is_running = &engine_state.is_running,
                    .engine_game_state = engine_state.game_state,
                    .user_input = user_input,
                    .matches = matches,
                };

                for (int j = 0; j < MAX_MATCHES && matches[j].rm_so != -1; j++) {
                    int start = matches[j].rm_so;
                    int end = matches[j].rm_eo;
                    cmd_params.matches[j] = strndup(user_input + start, end - start);
                }

                ENGINE_COMMANDS[i].func(cmd_params);
                printf("\n");
                regfree(&regex);

                for (int j = 0; j < MAX_MATCHES && cmd_params.matches[j]; j++) free(cmd_params.matches[j]);
                
                break;
            }

            regfree(&regex);
        }
    }   // while engine_state.is_running

    free_state(engine_state.game_state);
}