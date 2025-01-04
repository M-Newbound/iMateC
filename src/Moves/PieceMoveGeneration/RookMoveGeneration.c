/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../MoveGeneration.h"
#include "../MagicBitboards.h"

static flags_t make_rook_flags(uint64_t from_sq, color_t color) {
    // track which rook moved so castling rights can be revoked
    const uint64_t kingside_mask = (color == WHITE)
        ? 0x0000000000000080ULL   // h1
        : 0x8000000000000000ULL;  // h8
    const uint64_t queenside_mask = (color == WHITE)
        ? 0x0000000000000001ULL   // a1
        : 0x0100000000000000ULL;  // a8

    flags_t f = {
        .castle               = NULL_CASTLE,
        .promotion_piece      = NULL_PIECE,
        .en_passant_square    = 0,
        .king_moved           = false,
        .kingside_rook_moved  = (from_sq & kingside_mask)  != 0,
        .queenside_rook_moved = (from_sq & queenside_mask) != 0
    };
    return f;
}

void gen_rook_moves_on_square(const state_t *state, move_collection_t *collection,
                               uint64_t sq_bb) {
    const color_t color_to_move = side_to_move(state);
    const uint64_t own_bb = color_bb(state, color_to_move);
    const uint64_t opp_bb = color_bb(state, (color_to_move == WHITE) ? BLACK : WHITE);
    const uint64_t occupied = own_bb | opp_bb;

    flags_t flags = make_rook_flags(sq_bb, color_to_move);

    // magic lookup -- includes first blocker on each ray, excludes own pieces
    uint64_t attacks = magic_rook_attacks(__builtin_ctzll(sq_bb), occupied) & ~own_bb;

    while (attacks) {
        uint64_t to = attacks & (uint64_t)(-(int64_t)attacks); // isolate LSB
        push_move_to_collection(new_move(sq_bb, to, flags), collection);
        attacks &= attacks - 1; // clear LSB
    }
}
