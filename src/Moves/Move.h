/* iMate -- Copyright (C) 2024 Martin Newbound */

/*
 * Move.h - a single chess move and its associated flags.
 *
 * move_t is opaque. Create moves with new_move(), free them with free_move().
 */

#ifndef MOVE_H
#define MOVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../State/GameState.h"

typedef struct move move_t;

/* Flags encode everything about a move that can't be inferred from the
 * from/to squares alone: castling, promotions, en passant, rook tracking. */
typedef struct flags {
    piece_t  promotion_piece;       /* NULL_PIECE if not a promotion */
    castle_t castle;                /* NULL_CASTLE if not a castling move */
    uint64_t en_passant_square;     /* new EP target square (0 if none) */
    bool     king_moved;
    bool     kingside_rook_moved;
    bool     queenside_rook_moved;
} flags_t;

move_t         *new_move(uint64_t from_square, uint64_t to_square, flags_t flags);
void            free_move(move_t *move);
const flags_t  *get_move_flags(const move_t *move);
uint64_t        get_move_from_square(const move_t *move);
uint64_t        get_move_to_square(const move_t *move);

#ifdef __cplusplus
}
#endif

#endif /* MOVE_H */
