/* iMate -- Copyright (C) 2024 Martin Newbound */

/*
 * MoveGeneration.h - public interface for legal move generation.
 *
 * get_legal_moves_of_state returns all moves the side to move can legally
 * make. get_attacked_squares_bitboard returns a bitboard of every square
 * the opponent currently threatens (used for check detection and castling).
 */

#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include "../State/GameState.h"
#include "Move.h"
#include "MoveCollection.h"

move_collection_t *get_legal_moves_of_state(const state_t *state);
uint64_t           get_attacked_squares_bitboard(const state_t *state);

/* Flags for moves with no special attributes (knight, bishop, normal captures). */
static inline flags_t make_quiet_flags(void) {
    flags_t f = {
        .castle               = NULL_CASTLE,
        .promotion_piece      = NULL_PIECE,
        .en_passant_square    = 0,
        .king_moved           = false,
        .kingside_rook_moved  = false,
        .queenside_rook_moved = false
    };
    return f;
}

#endif /* MOVE_GEN_H */
