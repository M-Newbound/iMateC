/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include "../../Moves/MoveCollection.h"
#include "../../Moves/MoveGeneration.h"
#include <stdio.h>

// white uppercase, black lowercase
static const char PIECE_CHARS[2][6] = {
    {'P', 'R', 'N', 'B', 'Q', 'K'},
    {'p', 'r', 'n', 'b', 'q', 'k'}
};

static void print_board(const CommandParams params) {
    // rank 8 at the top, rank 1 at the bottom
    for (int rank = 7; rank >= 0; rank--) {
        printf("+---+---+---+---+---+---+---+---+\n");
        for (int file = 0; file < 8; file++) {
            uint64_t sq = 1ULL << (rank * 8 + file);
            piece_t piece = get_piece_on_square(params.engine_game_state, sq);
            if (piece == NULL_PIECE) {
                printf("|   ");
            } else {
                color_t color = get_color_of_piece_on_square(params.engine_game_state, sq);
                printf("| %c ", PIECE_CHARS[color][piece]);
            }
        }
        printf("| %d\n", rank + 1);
    }
    printf("+---+---+---+---+---+---+---+---+\n");
    printf("  a   b   c   d   e   f   g   h  \n");
}

static void print_moves(const CommandParams params, const char *square_str) {
    // e.g. "e4" -> bitboard square
    if (!square_str || square_str[0] < 'a' || square_str[0] > 'h'
                    || square_str[1] < '1' || square_str[1] > '8') {
        printf("Invalid square.\n");
        return;
    }
    int file = square_str[0] - 'a';
    int rank = square_str[1] - '1';
    uint64_t sq = 1ULL << (rank * 8 + file);

    move_collection_t *moves = get_legal_moves_of_state(params.engine_game_state);
    bool any = false;
    move_t *move;
    while ((move = pop_collection_head(moves)) != NULL) {
        if (get_move_from_square(move) == sq) {
            int to_idx = __builtin_ctzll(get_move_to_square(move));
            printf("%c%d\n", 'a' + (to_idx % 8), 1 + (to_idx / 8));
            any = true;
        }
        free_move(move);
    }
    free_move_collection(moves);

    if (!any) printf("No legal moves from %s.\n", square_str);
}

void print_command(const CommandParams params) {
    // [2]=board, [3]=moves, [4]=square e.g. "e4"
    if (params.matches[2] != NULL) {
        print_board(params);
    } else if (params.matches[3] != NULL) {
        print_moves(params, params.matches[4]);
    }
}
