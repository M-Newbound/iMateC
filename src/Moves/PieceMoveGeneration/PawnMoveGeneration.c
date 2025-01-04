/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../MoveGeneration.h"

// promotion rows
#define WHITE_PROMOTION_MASK 0xFF00000000000000ULL
#define BLACK_PROMOTION_MASK 0x00000000000000FFULL

// starting rows
#define WHITE_STARTING_ROW_MASK 0x000000000000FF00ULL
#define BLACK_STARTING_ROW_MASK 0x00FF000000000000ULL

// file boundary masks -- guard against wrapping on capture shifts
#define LHS_RANK_MASK 0x0101010101010101ULL  // a-file: guards CAPTURE_LEFT
#define RHS_RANK_MASK 0x8080808080808080ULL  // h-file: guards CAPTURE_RIGHT

// directional shifts
#define MOVE_FORWARD(BB, COLOR)   ((COLOR) == WHITE ? (BB) << 8 : (BB) >> 8)
#define CAPTURE_LEFT(BB, COLOR)   ((COLOR) == WHITE ? (BB) << 7 : (BB) >> 9)
#define CAPTURE_RIGHT(BB, COLOR)  ((COLOR) == WHITE ? (BB) << 9 : (BB) >> 7)

#define IS_ON_STARTING_ROW(BB, COLOR) \
    ((COLOR) == WHITE ? (BB) & WHITE_STARTING_ROW_MASK : (BB) & BLACK_STARTING_ROW_MASK)
#define IS_ON_PROMOTION_ROW(BB, COLOR) \
    ((COLOR) == WHITE ? (BB) & WHITE_PROMOTION_MASK : (BB) & BLACK_PROMOTION_MASK)

static flags_t make_pawn_flags(piece_t promo, uint64_t ep_sq) {
    flags_t f = {
        .castle               = NULL_CASTLE,
        .promotion_piece      = promo,
        .en_passant_square    = ep_sq,
        .king_moved           = false,
        .kingside_rook_moved  = false,
        .queenside_rook_moved = false
    };
    return f;
}

static bool handle_promotion_case(move_collection_t *collection, uint64_t from_sq,
                                   uint64_t to_sq, color_t color) {
    if (!IS_ON_PROMOTION_ROW(to_sq, color)) return false;

    for (piece_t p = PIECE_ROOK; p <= PIECE_QUEEN; p++) {
        flags_t f = make_pawn_flags(p, 0);
        push_move_to_collection(new_move(from_sq, to_sq, f), collection);
    }
    return true;
}

static void handle_single_move_forward(move_collection_t *collection, uint64_t sq_bb,
                                        color_t color, uint64_t all_pieces) {
    uint64_t fwd = MOVE_FORWARD(sq_bb, color) & ~all_pieces;
    if (!fwd) return;
    if (handle_promotion_case(collection, sq_bb, fwd, color)) return;
    flags_t f = make_pawn_flags(NULL_PIECE, 0);
    push_move_to_collection(new_move(sq_bb, fwd, f), collection);
}

static void handle_double_move_forward(move_collection_t *collection, uint64_t sq_bb,
                                        color_t color, uint64_t all_pieces) {
    if (!IS_ON_STARTING_ROW(sq_bb, color)) return;

    uint64_t fwd1 = MOVE_FORWARD(sq_bb, color) & ~all_pieces;
    if (!fwd1) return;
    uint64_t fwd2 = MOVE_FORWARD(fwd1, color) & ~all_pieces;
    if (!fwd2) return;

    // en passant target is the square the pawn passed through
    flags_t f = make_pawn_flags(NULL_PIECE, fwd1);
    push_move_to_collection(new_move(sq_bb, fwd2, f), collection);
}

static void handle_capturing(move_collection_t *collection, uint64_t sq_bb,
                               color_t color, uint64_t opp_bb) {
    uint64_t captures[2] = {CAPTURE_LEFT(sq_bb, color), CAPTURE_RIGHT(sq_bb, color)};
    uint64_t edges[2] = {LHS_RANK_MASK, RHS_RANK_MASK};

    for (int i = 0; i < 2; i++) {
        // skip if pawn is on the edge in this direction (would wrap)
        if (sq_bb & edges[i]) continue;
        if (!(captures[i] & opp_bb)) continue;

        if (!handle_promotion_case(collection, sq_bb, captures[i], color)) {
            flags_t f = make_pawn_flags(NULL_PIECE, 0);
            push_move_to_collection(new_move(sq_bb, captures[i], f), collection);
        }
    }
}

static void handle_en_passant(const state_t *state, move_collection_t *collection,
                               uint64_t sq_bb, color_t color) {
    if (!is_en_passant_target_active(state)) return;

    uint64_t ep_target = get_en_passant_target(state);
    uint64_t captures[2] = {CAPTURE_LEFT(sq_bb, color), CAPTURE_RIGHT(sq_bb, color)};
    uint64_t edges[2] = {LHS_RANK_MASK, RHS_RANK_MASK};

    for (int i = 0; i < 2; i++) {
        if (sq_bb & edges[i]) continue;
        if (captures[i] != ep_target) continue; // capture must land on the ep square
        flags_t f = make_pawn_flags(NULL_PIECE, 0);
        push_move_to_collection(new_move(sq_bb, captures[i], f), collection);
    }
}

void gen_pawn_moves_on_square(const state_t *state, move_collection_t *collection,
                               uint64_t sq_bb) {
    const color_t color = side_to_move(state);
    const color_t opp = (color == WHITE) ? BLACK : WHITE;
    const uint64_t opp_bb = color_bb(state, opp);
    const uint64_t all_pieces = color_bb(state, color) | opp_bb;

    handle_double_move_forward(collection, sq_bb, color, all_pieces);
    handle_single_move_forward(collection, sq_bb, color, all_pieces);
    handle_capturing(collection, sq_bb, color, opp_bb);
    handle_en_passant(state, collection, sq_bb, color);
}
