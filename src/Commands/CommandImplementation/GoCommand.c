/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include "../../Search/Search.h"
#include "../../Util/MoveParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// pull an integer value out of a UCI go string, e.g. parse_go_int("go depth 6 wtime 60000", "depth ") -> 6
// returns -1 if the keyword isn't present or isn't followed by a number
static int parse_go_int(const char *raw, const char *keyword) {
    const char *p = strstr(raw, keyword);
    if (!p) return -1;
    p += strlen(keyword);
    while (*p == ' ' || *p == '\t') p++;
    if (*p < '0' || *p > '9') return -1;
    return atoi(p);
}

void go_command(const CommandParams params) {
    const char *raw = params.raw_input;
    SearchLimits limits = {0, 0};

    // depth param
    int depth = parse_go_int(raw, "depth ");
    if (depth > 0) limits.max_depth = depth;

    // movetime param
    int movetime = parse_go_int(raw, "movetime ");
    if (movetime > 0) limits.movetime_ms = movetime;

    // infinite: search to max depth, no time limit
    if (strstr(raw, "infinite")) {
        limits.max_depth = SEARCH_MAX_DEPTH;
        limits.movetime_ms = 0;
    }

    // time control allocation when no explicit budget given
    if (limits.movetime_ms == 0 && limits.max_depth == 0) {
        color_t c = side_to_move(params.engine_game_state);
        int time_left = parse_go_int(raw, c == WHITE ? "wtime " : "btime ");
        int increment = parse_go_int(raw, c == WHITE ? "winc " : "binc ");
        int movestogo = parse_go_int(raw, "movestogo ");

        if (time_left > 0) {
            int divisor = (movestogo > 0) ? movestogo : 20;
            int inc_bonus = (increment > 0) ? increment / 2 : 0;
            int alloc = time_left / divisor + inc_bonus;
            // floor at 50ms so depth 1 always finishes
            limits.movetime_ms = alloc > 50 ? alloc : 50;
        }
    }

    move_t *best = do_move_search(params.engine_game_state, limits);

    if (!best) {
        printf("bestmove (none)\n");
        fflush(stdout);
        return;
    }

    char mv_str[6];
    move_to_uci_str(mv_str, best);
    printf("bestmove %s\n", mv_str);
    fflush(stdout);
    free_move(best);
}
