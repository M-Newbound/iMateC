/* iMate -- Copyright (C) 2024 Martin Newbound */

/*
 * Search.h - negamax alpha-beta with iterative deepening.
 *
 * Zero-initialise SearchLimits to use engine defaults (5 plies, no time limit).
 * do_move_search prints a UCI info line after each completed depth and returns
 * the best move found. Caller must free the returned move, or handle NULL
 * if there are no legal moves.
 */

#ifndef SEARCH_H
#define SEARCH_H

#include "../State/GameState.h"
#include "../Moves/Move.h"

#define SEARCH_MAX_DEPTH 64

typedef struct {
    int max_depth;    /* plies to search; 0 = use default (5) */
    int movetime_ms;  /* wall-clock budget in ms; 0 = no limit */
} SearchLimits;

move_t *do_move_search(const state_t *state, SearchLimits limits);

#endif /* SEARCH_H */
