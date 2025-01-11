/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include <stdio.h>

#define CMD_WIDTH  36
#define DESC_WIDTH 44

static const char *CMD_DESCRIPTIONS[][2] = {
    // UCI protocol
    {"uci",                                    "Identify engine; respond with uciok"},
    {"isready",                                "Confirm engine is ready (readyok)"},
    {"ucinewgame",                             "Reset to starting position"},
    {"stop",                                   "Halt an ongoing search"},
    // UCI game commands
    {"position startpos [moves m1 m2 ...]",    "Set starting position, optionally apply moves"},
    {"position fen <fen> [moves m1 m2 ...]",   "Set arbitrary FEN, optionally apply moves"},
    {"go [depth n] [movetime ms]",             "Search and print bestmove"},
    {"go wtime w btime b [winc i] [binc i]",   "Search with time controls"},
    // interactive
    {"print board",                            "Display the current board"},
    {"print moves <sq>",                       "List legal moves from a square (e.g. e2)"},
    {"move <from><to>[promo]",                 "Apply a move interactively (e.g. e2e4)"},
    {"status",                                 "Show check / checkmate / stalemate"},
    {"help",                                   "Show this table"},
    {"quit",                                   "Exit the engine"},
};

#define LENGTH_OF_COMMANDS (sizeof(CMD_DESCRIPTIONS) / sizeof(CMD_DESCRIPTIONS[0]))

static void print_separator(void) {
    printf("+");
    for (int i = 0; i < CMD_WIDTH; i++) printf("-");
    printf("+");
    for (int i = 0; i < DESC_WIDTH; i++) printf("-");
    printf("+\n");
}

void help_command(const CommandParams params) {
    (void)params;
    print_separator();
    printf("| %-*s | %-*s |\n", CMD_WIDTH - 2, "Command", DESC_WIDTH - 2, "Description");
    print_separator();
    for (size_t i = 0; i < LENGTH_OF_COMMANDS; i++) {
        printf("| %-*s | %-*s |\n",
               CMD_WIDTH - 2, CMD_DESCRIPTIONS[i][0],
               DESC_WIDTH - 2, CMD_DESCRIPTIONS[i][1]);
        print_separator();
    }
}
