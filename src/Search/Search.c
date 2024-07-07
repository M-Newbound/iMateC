/* iMate -- Copyright (C) 2024 Martin Newbound */                                                    

#include "Search.h"
#include "../Moves/Move.h"
#include "../Moves/MoveCollection.h"
#include "../Moves/MoveGeneration.h"
#include "../State/GameState.h"
#include "../Evaluation/Evaluation.h"

#include <limits.h>
#include <stddef.h>

/**
 * The minimax function is a recursive function that uses the minimax algorithm with alpha-beta pruning 
 * to search the game tree for the best move.
 *
 * @param state The current game state.
 * @param depth The maximum depth to search to.
 * @param alpha The best (highest) score that the calling function can guarantee at this level or above.
 * @param beta The worst (lowest) score that the calling function can guarantee at the next level or above.
 * @param best_move A pointer to a move_t struct where the best move will be stored.
 * @return The score of the best move.
 */
float minimax(const state_t *state, int depth, int alpha, int beta, move_t *best_move) {

    // Base case: if we've reached the maximum depth, evaluate the state and return the score.
    if (depth == 0) return evaluate_state(state);
    
    // Generate all possible moves from the current state.
    move_collection_t *moves = generate_psudo_legal_moves(state);
    prune_illegal_moves(state, moves);

    int max_eval = INT_MIN;
    move_t *max_move = NULL;

    // Iterate over all moves.
    move_t *curr_move = pop_collection_head(moves);
    while (curr_move != NULL) {
        // Apply the current move to a copy of the state.
        state_t *tmp_state = new_state();
        copy_state(state, tmp_state);
        apply_move(curr_move, tmp_state);

        // Recursively call minimax on the new state.
        float eval = -1 * minimax(tmp_state, depth - 1, -beta, -alpha, NULL);
        free_state(tmp_state);

        // If this move is better than the current best move, update the best move and the best score.
        if (max_move == NULL || eval > max_eval) {
            if (max_move != NULL) free_move(max_move);
            max_move = curr_move;
            max_eval = eval;
        } 
        
        // If this move is not better than the current best move, free the memory allocated for it.
        else free_move(curr_move);

        // Update alpha (the best score that we can guarantee at this level or above).
        alpha = alpha > eval ? alpha : eval;

        // If alpha is greater than or equal to beta, prune this branch.
        if (alpha >= beta) break;

        curr_move = pop_collection_head(moves);
        
    }    

    // Free the memory allocated for the move collection.
    free_move_collection(moves);

    // Store the best move in the best_move parameter.
    best_move = max_move;
    
    return max_eval;
}

/**
 * The do_move_search function is the entry point for the search. It calls the minimax function 
 * with the initial alpha and beta values.
 *
 * @param state The current game state.
 * @param best_move A pointer to a move_t struct where the best move will be stored.
 * @param max_depth The maximum depth to search to.
 */
void do_move_search(const state_t *state, move_t *best_move, int max_depth) {
    minimax(state, max_depth, INT_MIN, INT_MAX, best_move);
}