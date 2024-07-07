/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

/**
 * @brief Executes the 'position' command.
 *
 * This function is responsible for setting the state of the engine's internal game board.
 * The board state is set based on the command parameters provided.
 * 
 * @param params The command parameters, including the current game state and the FEN string representing the new board state.
 *
 * @todo Implement the function to parse the FEN string and set the game state.
 */
void position_command(const CommandParams params) {
    // The following line is commented out because the 'fen' variable is not defined.
    // load_fen_string(params.engine_game_state, fen);
}