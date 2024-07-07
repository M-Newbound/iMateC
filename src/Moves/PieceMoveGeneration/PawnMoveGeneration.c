/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../MoveGeneration.h"


// Masks for identifying promotion rows for white and black pawns
#define WHITE_PROMOTION_MASK 0xFF00000000000000
#define BLACK_PROMOTION_MASK 0x00000000000000FF

// Masks for identifying starting rows for white and black pawns
#define WHITE_STARTING_ROW_MASK 0x000000000000FF00
#define BLACK_STARTING_ROW_MASK 0x00FF000000000000

// Macros for identifying left and right side of the board depending on the color
#define LHS_RANK_MASK(COLOR) ((COLOR == WHITE) ? 0x00000000000000FF : 0xFF00000000000000)
#define RHS_RANK_MASK(COLOR) ((COLOR == WHITE) ? 0x000000000000FF00 : 0x00FF000000000000)

// Macros for moving a pawn forward and capturing to the left or right depending on the color
#define MOVE_FORWARD(BITBOARD, COLOR) ((COLOR == WHITE) ? BITBOARD << 8 : BITBOARD >> 8)
#define CAPTURE_LEFT(BITBOARD, COLOR) ((COLOR == WHITE) ? BITBOARD << 7 : BITBOARD >> 9)
#define CAPTURE_RIGHT(BITBOARD, COLOR) ((COLOR == WHITE) ? BITBOARD << 9 : BITBOARD >> 7)

// Macros for moving a pawn to the left or right on the same rank depending on the color
#define LEFT_ON_RANK(BITBOARD, COLOR) ((COLOR == WHITE) ? BITBOARD << 1 : BITBOARD >> 1)
#define RIGHT_ON_RANK(BITBOARD, COLOR) ((COLOR == WHITE) ? BITBOARD >> 1 : BITBOARD << 1)

// Macros for checking if a pawn is on its starting or promotion row depending on the color
#define IS_ON_STARTING_ROW(BITBOARD, COLOR) ((COLOR == WHITE) ? (BITBOARD & WHITE_STARTING_ROW_MASK) : (BITBOARD & BLACK_STARTING_ROW_MASK))
#define IS_ON_PROMOTION_ROW(BITBOARD, COLOR) ((COLOR == WHITE) ? (BITBOARD & WHITE_PROMOTION_MASK) : (BITBOARD & BLACK_PROMOTION_MASK))

/**
 * Creates a flags_t struct with the given parameters.
 * @param double_pawn_push A boolean indicating if the move is a double pawn push.
 * @param promotion_piece The piece that the pawn is promoted to.
 * @return A flags_t struct with the given parameters.
 */
flags_t create_pawn_flags(bool double_pawn_push, piece_t promotion_piece) {
    flags_t flags = {
        .castle = NULL_CASTLE,
        .double_pawn_push = double_pawn_push,
        .promotion_piece = promotion_piece,
        .king_moved = false,
        .kingside_rook_moved = false,
        .queenside_rook_moved = false
    };
    return flags;
}

/**
 * Handles the case where a pawn reaches the promotion row.
 * @param collection The collection of moves.
 * @param square_key The key of the square where the pawn is located.
 * @param to_square The square where the pawn is moving to.
 * @param color_to_move The color of the pawn.
 * @return A boolean indicating if the pawn reached the promotion row.
 */
bool handle_promotion_case(move_collection_t *collection, uint64_t square_key, uint64_t to_square, color_t color_to_move) {
    if(!IS_ON_PROMOTION_ROW(to_square, color_to_move)) return false;

    for (piece_t piece = PIECE_ROOK; piece <= PIECE_QUEEN; ++piece) {
        flags_t flags = create_pawn_flags(false, piece);
        move_t const *move = new_move(square_key, to_square, flags);
        push_move_to_collection(move, collection);
    }

    return true;
}

/**
 * Handles the case where a pawn moves forward by one square.
 * @param collection The collection of moves.
 * @param square_key The key of the square where the pawn is located.
 * @param color_to_move The color of the pawn.
 * @param opponent_bitboard The bitboard of the opponent.
 */
void handle_single_move_forward(move_collection_t *collection, uint64_t square_key, color_t color_to_move, uint64_t opponent_bitboard) {
    uint64_t forward_one = MOVE_FORWARD(square_key, color_to_move);
    forward_one &= ~opponent_bitboard;  // make sure it is not blocked

    if (forward_one == 0) return;
    if (handle_promotion_case(collection, square_key, forward_one, color_to_move)) return;

    flags_t flags = create_pawn_flags(false, NULL_PIECE);
    move_t const *move = new_move(square_key, forward_one, flags);
    push_move_to_collection(move, collection);
}

/**
 * Handles the case where a pawn moves forward by two squares.
 * @param collection The collection of moves.
 * @param square_key The key of the square where the pawn is located.
 * @param color_to_move The color of the pawn.
 * @param opponent_bitboard The bitboard of the opponent.
 */
void handle_double_move_forward(move_collection_t *collection, uint64_t square_key, color_t color_to_move, uint64_t opponent_bitboard) {
    if (!IS_ON_STARTING_ROW(square_key, color_to_move)) return;
    
    uint64_t forward_one = MOVE_FORWARD(square_key, color_to_move);
    forward_one &= ~opponent_bitboard;  // make sure it is not blocked

    uint64_t forward_two = MOVE_FORWARD(forward_one, color_to_move);
    forward_two &= ~opponent_bitboard;  // make sure it is not blocked

    if (forward_two == 0) return;

    flags_t flags = create_pawn_flags(true, NULL_PIECE);
    move_t const *move = new_move(square_key, forward_two, flags);
    push_move_to_collection(move, collection);
}

/**
 * Handles the case where a pawn captures an opponent's piece.
 * @param collection The collection of moves.
 * @param square_key The key of the square where the pawn is located.
 * @param color_to_move The color of the pawn.
 * @param opponent_bitboard The bitboard of the opponent.
 */
void handle_capturing(move_collection_t *collection, uint64_t square_key, color_t color_to_move, uint64_t opponent_bitboard) {
    flags_t flags = create_pawn_flags(false, NULL_PIECE);

    uint64_t capture_moves[] = {CAPTURE_LEFT(square_key, color_to_move), CAPTURE_RIGHT(square_key, color_to_move)};
    uint64_t rank_masks[] = {LHS_RANK_MASK(color_to_move), RHS_RANK_MASK(color_to_move)};

    for (int i = 0; i < 2; ++i) {
        if ((square_key & rank_masks[i]) == 0 && (capture_moves[i] & opponent_bitboard)) {
            move_t const *move = new_move(square_key, capture_moves[i], flags);
            push_move_to_collection(move, collection);
        }
    }
}

/**
 * Handles the case where a pawn captures an opponent's pawn en passant.
 * @param state The current state of the game.
 * @param collection The collection of moves.
 * @param square_key The key of the square where the pawn is located.
 * @param color_to_move The color of the pawn.
 */
void handle_en_passant(const state_t *state, move_collection_t *collection, uint64_t square_key, color_t color_to_move) {
    if (!is_en_passant_target_active(state)) return;
    uint64_t en_passant_target = get_en_passant_target(state);

    flags_t flags = create_pawn_flags(false, NULL_PIECE);

    uint64_t capture_moves[] = {CAPTURE_LEFT(square_key, color_to_move), CAPTURE_RIGHT(square_key, color_to_move)};
    uint64_t rank_masks[] = {LHS_RANK_MASK(color_to_move), RHS_RANK_MASK(color_to_move)};
    uint64_t rank_moves[] = {LEFT_ON_RANK(square_key, color_to_move), RIGHT_ON_RANK(square_key, color_to_move)};

    for (int i = 0; i < 2; ++i) {
        if ((square_key & rank_masks[i]) == 0 && en_passant_target == rank_moves[i]) {
            move_t const *move = new_move(square_key, capture_moves[i], flags);
            push_move_to_collection(move, collection);
        }
    }
}

/**
 * Generates all possible moves for a pawn on a given square.
 * @param state The current state of the game.
 * @param collection The collection of moves.
 * @param square_key The key of the square where the pawn is located.
 */
void gen_pawn_moves_on_square(const state_t *state, move_collection_t *collection, uint64_t square_key) {
    const color_t color_to_move = get_state_to_move_color(state);
    const uint64_t opponent_bitboard = states_color_bitboard(state, (color_to_move == WHITE) ? BLACK : WHITE);

    handle_double_move_forward(collection, square_key, color_to_move, opponent_bitboard);
    handle_single_move_forward(collection, square_key, color_to_move, opponent_bitboard);
    handle_capturing(collection, square_key, color_to_move, opponent_bitboard);
    handle_en_passant(state, collection, square_key, color_to_move);
}