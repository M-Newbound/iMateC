/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../MoveGeneration.h"
#include <stdlib.h>

static flags_t king_base_flags(void) {
    flags_t f = {
        .castle               = NULL_CASTLE,
        .promotion_piece      = NULL_PIECE,
        .en_passant_square    = 0,
        .king_moved           = true,
        .kingside_rook_moved  = false,
        .queenside_rook_moved = false
    };
    return f;
}

// plain king step moves -- just the 8 neighbours, no castling
// used by the attack map generator to avoid recursion through
// handle_castling -> get_attacked_squares_bitboard -> here
void gen_king_attacks_on_square(const state_t *state, move_collection_t *collection,
                                 uint64_t sq_bb) {
    const color_t color_to_move = side_to_move(state);
    const uint64_t own_bb = color_bb(state, color_to_move);
    int sq = __builtin_ctzll(sq_bb);
    int sq_file = sq % 8;
    flags_t flags = king_base_flags();

    const int offsets[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    const int num_offsets = (int)(sizeof(offsets) / sizeof(offsets[0]));

    for (int i = 0; i < num_offsets; i++) {
        int to_sq = sq + offsets[i];
        if (to_sq < 0 || to_sq >= 64) continue;
        if (abs((to_sq % 8) - sq_file) > 1) continue;
        uint64_t to_bb = 1ULL << to_sq;
        if (own_bb & to_bb) continue;
        push_move_to_collection(new_move(sq_bb, to_bb, flags), collection);
    }
}

static void handle_castling(move_collection_t *collection, const state_t *state,
                             color_t color_to_move, uint64_t king_sq) {
    flags_t base = king_base_flags();

    // can't castle out of, through, or into check
    uint64_t attacked = get_attacked_squares_bitboard(state);

    if (color_to_move == WHITE) {
        // e1|f1|g1 must all be safe
        if (state_can_castle(state, CASTLE_KINGSIDE_WHITE) &&
            !(attacked & 0x0000000000000070ULL)) {
            flags_t f = base; f.castle = CASTLE_KINGSIDE_WHITE;
            push_move_to_collection(
                new_move(king_sq, 0x0000000000000040ULL, f), collection); // ->g1
        }
        // e1|d1|c1 must all be safe
        if (state_can_castle(state, CASTLE_QUEENSIDE_WHITE) &&
            !(attacked & 0x000000000000001CULL)) {
            flags_t f = base; f.castle = CASTLE_QUEENSIDE_WHITE;
            push_move_to_collection(
                new_move(king_sq, 0x0000000000000004ULL, f), collection); // ->c1
        }
    } else {
        // e8|f8|g8 must all be safe
        if (state_can_castle(state, CASTLE_KINGSIDE_BLACK) &&
            !(attacked & 0x7000000000000000ULL)) {
            flags_t f = base; f.castle = CASTLE_KINGSIDE_BLACK;
            push_move_to_collection(
                new_move(king_sq, 0x4000000000000000ULL, f), collection); // ->g8
        }
        // e8|d8|c8 must all be safe
        if (state_can_castle(state, CASTLE_QUEENSIDE_BLACK) &&
            !(attacked & 0x1C00000000000000ULL)) {
            flags_t f = base; f.castle = CASTLE_QUEENSIDE_BLACK;
            push_move_to_collection(
                new_move(king_sq, 0x0400000000000000ULL, f), collection); // ->c8
        }
    }
}

void gen_king_moves_on_square(const state_t *state, move_collection_t *collection,
                               uint64_t sq_bb) {
    gen_king_attacks_on_square(state, collection, sq_bb);
    handle_castling(collection, state, side_to_move(state), sq_bb);
}
