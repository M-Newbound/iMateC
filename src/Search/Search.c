/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "Search.h"
#include "../Moves/Move.h"
#include "../Moves/MoveCollection.h"
#include "../Moves/MoveGeneration.h"
#include "../State/GameState.h"
#include "../Evaluation/Evaluation.h"
#include "../Util/MoveParser.h"

#include <float.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#define SEARCH_DEFAULT_DEPTH 5

// centipawn values for MVV-LVA capture ordering
static const int CAPTURE_VALUE[6] = {100, 500, 320, 330, 900, 20000};
/*                                   PAW  ROK  KNI  BIS  QUE  KIN  */

// search state, reset at the start of every root call
static long long g_nodes;
static bool g_stopped;
static long long g_stop_time_ms; // 0 = no time limit

static long long now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

// MVV-LVA: score captures by victim value minus attacker value
// higher score = search this capture first, non-captures score 0
static int mvv_lva_score(const move_t *move, const state_t *state) {
    piece_t victim = get_piece_on_square(state, get_move_to_square(move));
    if (victim == NULL_PIECE) return 0;

    piece_t attacker = get_piece_on_square(state, get_move_from_square(move));
    int attacker_val = (attacker != NULL_PIECE) ? CAPTURE_VALUE[(int)attacker] : 0;
    return CAPTURE_VALUE[(int)victim] * 10 - attacker_val;
}

// insertion sort -- optimal for n <= 218 (max legal moves in chess)
static void sort_moves(move_t **arr, int *scores, int n) {
    for (int i = 1; i < n; i++) {
        move_t *key = arr[i];
        int key_s = scores[i];
        int j = i - 1;
        while (j >= 0 && scores[j] < key_s) {
            arr[j + 1] = arr[j];
            scores[j + 1] = scores[j];
            j--;
        }
        arr[j + 1] = key;
        scores[j + 1] = key_s;
    }
}

/**
 * Negamax with alpha-beta pruning. Scores are always from the perspective
 * of the side to move -- callers negate when recursing.
 */
static float minimax(const state_t *state, int depth, float alpha, float beta,
                     move_t **best_move_out) {
    g_nodes++;

    // poll the clock every 2048 nodes to keep overhead low
    if (g_stop_time_ms && (g_nodes & 2047) == 0) {
        if (now_ms() >= g_stop_time_ms) g_stopped = true;
    }
    if (g_stopped) return 0.0f;

    move_collection_t *moves = get_legal_moves_of_state(state);

    if (move_collection_is_empty(moves)) {
        free_move_collection(moves);
        return is_check(state, side_to_move(state))
               ? -FLT_MAX  // checkmate
               : 0.0f;     // stalemate
    }

    if (depth == 0) {
        free_move_collection(moves);
        return evaluate_state(state);
    }

    // score and sort moves so captures are tried first
    move_t *move_arr[218];
    int scores[218];
    int n_moves = 0;

    move_t *m;
    while ((m = pop_collection_head(moves)) != NULL)
        move_arr[n_moves++] = m;
    free_move_collection(moves);

    for (int i = 0; i < n_moves; i++)
        scores[i] = mvv_lva_score(move_arr[i], state);
    sort_moves(move_arr, scores, n_moves);

    float best_score = -FLT_MAX;
    move_t *best_move = NULL;

    for (int i = 0; i < n_moves; i++) {
        m = move_arr[i];

        state_t *tmp = clone_state(state);
        play_move(tmp, m);
        float score = -minimax(tmp, depth - 1, -beta, -alpha, NULL);
        free_state(tmp);

        if (score > best_score) {
            best_score = score;
            if (best_move) free_move(best_move);
            best_move = m;
        } else {
            free_move(m);
        }

        if (best_score > alpha) alpha = best_score;
        if (alpha >= beta) {
            for (int j = i + 1; j < n_moves; j++) free_move(move_arr[j]);
            break;
        }
    }

    if (best_move_out) {
        *best_move_out = best_move;
    } else if (best_move) {
        free_move(best_move);
    }
    return best_score;
}

/**
 * Iterative deepening with time management.
 * Searches depth 1..max_depth. If time runs out mid-depth the partial result
 * is discarded and we return the best move from the last completed depth.
 */
move_t *do_move_search(const state_t *state, SearchLimits limits) {
    int max_depth = (limits.max_depth > 0) ? limits.max_depth : SEARCH_DEFAULT_DEPTH;

    g_nodes = 0;
    g_stopped = false;
    g_stop_time_ms = (limits.movetime_ms > 0)
                     ? now_ms() + (long long)limits.movetime_ms
                     : 0;

    long long search_start = now_ms();
    move_t *best = NULL;

    for (int depth = 1; depth <= max_depth && !g_stopped; depth++) {
        move_t *candidate = NULL;
        float score = minimax(state, depth, -FLT_MAX, FLT_MAX, &candidate);

        if (g_stopped) {
            // incomplete depth -- discard partial result
            if (candidate) free_move(candidate);
            break;
        }

        if (best) free_move(best);
        best = candidate;

        if (!best) break;

        long long elapsed = now_ms() - search_start;
        long long nps = (elapsed > 0) ? g_nodes * 1000LL / elapsed : 0;
        char pv_str[6];
        move_to_uci_str(pv_str, best);

        if (score > FLT_MAX / 2.0f)
            printf("info depth %d score mate 1 nodes %lld nps %lld time %lld pv %s\n",
                   depth, g_nodes, nps, elapsed, pv_str);
        else if (score < -FLT_MAX / 2.0f)
            printf("info depth %d score mate -1 nodes %lld nps %lld time %lld pv %s\n",
                   depth, g_nodes, nps, elapsed, pv_str);
        else
            printf("info depth %d score cp %d nodes %lld nps %lld time %lld pv %s\n",
                   depth, (int)score, g_nodes, nps, elapsed, pv_str);
        fflush(stdout);

        // found a forced mate, no point going deeper
        if (score > FLT_MAX / 2.0f || score < -FLT_MAX / 2.0f) break;
    }

    return best;
}
