/* iMate -- Copyright (C) 2024 Martin Newbound */

/*
 * MoveParser.h - UCI move string utilities.
 *
 * Handles conversion between UCI long-algebraic notation ("e2e4", "e7e8q")
 * and the internal bitboard representation. Used by the position command
 * (move sequences), move command (interactive play), and go command
 * (formatting the bestmove response).
 */

#ifndef MOVE_PARSER_H
#define MOVE_PARSER_H

#include "../State/GameState.h"
#include "../Moves/Move.h"
#include <stdbool.h>

/* Write a UCI square string ("e4\0") into buf. buf needs at least 3 bytes. */
void sq_to_uci_str(char *buf, uint64_t sq);

/* Write a UCI move string ("e7e8q\0") into buf. buf needs at least 6 bytes. */
void move_to_uci_str(char *buf, const move_t *move);

/* Parse and apply a UCI move string to state. Returns false if the move
 * is illegal or malformed. Defaults to queen when no promotion is specified. */
bool apply_uci_move(state_t *state, const char *move_str);

#endif /* MOVE_PARSER_H */
