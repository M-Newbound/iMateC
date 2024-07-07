/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Prints the current state of the game board.
 *
 * This function is responsible for printing the current state of the game board.
 * 
 * @param params The command parameters, including the current game state.
 */
void print_board(const CommandParams params) {
    for (size_t i = 0; i < 64; i++) {
        piece_t piece = get_piece_on_square(params.engine_game_state, 1 << i);
        color_t color = get_color_of_piece_on_square(params.engine_game_state, 1 << i);

        if (i % 8 == 0) printf("+---+---+---+---+---+---+---+---+\n");

        switch (piece) {
            case PIECE_PAWN:
                printf(color == WHITE ? "| P " : "| p ");
                break;
            case PIECE_KNIGHT:
                printf(color == WHITE ? "| N " : "| n ");
                break;
            case PIECE_BISHOP:
                printf(color == WHITE ? "| B " : "| b ");
                break;
            case PIECE_ROOK:
                printf(color == WHITE ? "| R " : "| r ");
                break;
            case PIECE_QUEEN:
                printf(color == WHITE ? "| Q " : "| q ");
                break;
            case PIECE_KING:
                printf(color == WHITE ? "| K " : "| k ");
                break;
            default:
                printf("|   ");
                break;
        }

        if ((i + 1) % 8 == 0) printf("| %i\n", (int)(8 - (i / 8)));
    }

    printf("+---+---+---+---+---+---+---+---+\n");
    printf("  a   b   c   d   e   f   g   h  \n");
}

/**
 * @brief Prints all possible moves from a square.
 *
 * This function is responsible for printing all possible moves from a square.
 * 
 * @param params The command parameters, including the current game state.
 * @param square The square from which to print all possible moves.
 *
 * @todo Implement the function to print all possible moves from a square.
 */
void print_moves(const CommandParams params, const char *square) {

}

/**
 * @brief Executes the 'print' command.
 *
 * This function is responsible for executing the 'print' command.
 * The command can either print the current state of the game board or print all possible moves from a square.
 * 
 * @param params The command parameters, including the current game state and any additional parameters.
 */
void print_command(const CommandParams params) {
    if (params.matches[2] == NULL) {                // The command is "print board"
        print_board(params);
    } else if (params.matches[3] != NULL) {         // The command is "print moves x"
        print_moves(params, params.matches[3]);
    }
}