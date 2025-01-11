/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include "../../Util/MoveParser.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void position_command(const CommandParams params) {
    // skip "position" and surrounding whitespace
    const char *p = params.raw_input;
    while (*p && !isspace((unsigned char)*p)) p++;
    while (*p && isspace((unsigned char)*p)) p++;

    // copy rest into a mutable buffer and trim trailing newline/spaces
    // sized to the actual input -- a full game's move list (resent on every
    // "position" command per the UCI protocol) can far exceed a fixed buffer
    size_t rest_cap = strlen(p) + 1;
    char *rest = malloc(rest_cap);
    memcpy(rest, p, rest_cap);
    int len = (int)strlen(rest);
    while (len > 0 && (rest[len-1] == '\n' || rest[len-1] == '\r'
                    || rest[len-1] == ' '  || rest[len-1] == '\t'))
        rest[--len] = '\0';

    // split on " moves " - FEN fields can't contain "moves" so this is unambiguous
    const char *moves_start = NULL;
    char *mp = strstr(rest, " moves ");
    if (mp) {
        *mp = '\0';
        moves_start = mp + 7;
    }

    // load position
    const char *pos = rest;

    if (pos[0] == '\0' || strcmp(pos, "startpos") == 0) {
        load_fen_string(params.engine_game_state, START_FEN);

    } else if (strncmp(pos, "fen ", 4) == 0) {
        const char *fen = pos + 4;
        while (isspace((unsigned char)*fen)) fen++;
        if (*fen == '\0') {
            printf("Usage: position fen <fen-string> [moves <m1> ...]\n");
            free(rest);
            return;
        }
        load_fen_string(params.engine_game_state, fen);

    } else {
        printf("Usage: position [startpos | fen <fen>] [moves <m1> ...]\n");
        free(rest);
        return;
    }

    // apply moves if any
    if (moves_start) {
        const char *mp2 = moves_start;
        char mv[6];
        while (*mp2) {
            while (isspace((unsigned char)*mp2)) mp2++;
            if (!*mp2) break;

            int i = 0;
            while (*mp2 && !isspace((unsigned char)*mp2) && i < 5)
                mv[i++] = *mp2++;
            mv[i] = '\0';

            if (i >= 4 && !apply_uci_move(params.engine_game_state, mv)) {
                printf("Illegal move in sequence: %s\n", mv);
                free(rest);
                return;
            }
        }
    }

    printf("Position set.\n");
    free(rest);
}
