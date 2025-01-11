/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include <stdio.h>

// tell the GUI who we are
void uci_command(const CommandParams params) {
    (void)params;
    printf("id name iMateC\n");
    printf("id author Martin Newbound\n");
    printf("uciok\n");
    fflush(stdout);
}

// confirm we're ready
void isready_command(const CommandParams params) {
    (void)params;
    printf("readyok\n");
    fflush(stdout);
}

void ucinewgame_command(const CommandParams params) {
    load_fen_string(params.engine_game_state, START_FEN);
}

// no-op in a single-threaded engine - search already finished before we get here
void stop_command(const CommandParams params) {
    (void)params;
}
