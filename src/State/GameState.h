/* iMate -- Copyright (C) 2024 Martin Newbound */

/*
 * GameState.h - everything needed to represent a chess position.
 *
 * The state_t type is opaque; the functions below are the only way to
 * read or modify it. This keeps the bitboard layout private so callers
 * don't need to know which index maps to which piece.
 */

#ifndef GAME_STATE_H
#define GAME_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Forward-declare move_t so Move.h doesn't need to be included here
 * (that would create a circular dependency). */
typedef struct move move_t;

typedef enum {
    NULL_COLOR  = -1,
    WHITE       =  0,
    BLACK       =  1
} color_t;

typedef enum {
    NULL_PIECE   = -1,
    PIECE_PAWN   =  0,
    PIECE_ROOK   =  1,
    PIECE_KNIGHT =  2,
    PIECE_BISHOP =  3,
    PIECE_QUEEN  =  4,
    PIECE_KING   =  5
} piece_t;

typedef enum {
    NULL_CASTLE            = -1,
    CASTLE_KINGSIDE_WHITE  =  0,
    CASTLE_QUEENSIDE_WHITE =  1,
    CASTLE_KINGSIDE_BLACK  =  2,
    CASTLE_QUEENSIDE_BLACK =  3
} castle_t;

typedef enum {
    IN_GAME,
    WHITE_CHECK,
    BLACK_CHECK,
    WHITE_CHECKMATE,
    BLACK_CHECKMATE,
    STALEMATE
} game_status_t;

/* Opaque - use the accessors below. */
typedef struct state state_t;

/* Lifecycle */
state_t *new_state(void);
void     free_state(state_t *state);
state_t *clone_state(const state_t *src);

/* Load a FEN string, replacing whatever position was there. */
void load_fen_string(state_t *state, const char *fen);

/* Apply a move in-place (updates bitboards, clocks, side to move). */
void play_move(state_t *state, const move_t *move);

/* Basic queries */
color_t  side_to_move(const state_t *state);
void     set_side_to_move(state_t *state, color_t color);
piece_t  get_piece_on_square(const state_t *state, uint64_t square);
color_t  get_color_of_piece_on_square(const state_t *state, uint64_t square);
uint64_t color_bb(const state_t *state, color_t color);
uint64_t piece_bb(const state_t *state, piece_t piece, color_t color);
uint64_t get_en_passant_target(const state_t *state);
bool     is_en_passant_target_active(const state_t *state);

/* Returns true if castling rights exist and the path is clear.
 * Does NOT check whether the king passes through check - that's handled
 * in KingMoveGeneration via get_attacked_squares_bitboard. */
bool state_can_castle(const state_t *state, castle_t castle);

game_status_t get_state_status(const state_t *state);

bool is_check(const state_t *state, color_t color);
bool is_checkmate(const state_t *state, color_t color);
bool is_stalemate(const state_t *state, color_t color);

#ifdef __cplusplus
}
#endif

#endif /* GAME_STATE_H */
