/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "MoveParser.h"
#include "../Moves/MoveCollection.h"
#include "../Moves/MoveGeneration.h"
#include <string.h>
#include <ctype.h>

// indexed by piece_t (PAWN=0 .. KING=5)
static const char PROMO_CHARS[] = {'p', 'r', 'n', 'b', 'q', 'k'};

void sq_to_uci_str(char *buf, uint64_t sq) {
    int idx = __builtin_ctzll(sq);
    buf[0] = 'a' + (idx % 8);
    buf[1] = '1' + (idx / 8);
    buf[2] = '\0';
}

void move_to_uci_str(char *buf, const move_t *move) {
    sq_to_uci_str(buf,     get_move_from_square(move));
    sq_to_uci_str(buf + 2, get_move_to_square(move));
    buf[4] = '\0';

    const flags_t *f = get_move_flags(move);
    if (f->promotion_piece != NULL_PIECE) {
        buf[4] = PROMO_CHARS[(int)f->promotion_piece];
        buf[5] = '\0';
    }
}

bool apply_uci_move(state_t *state, const char *move_str) {
    if (!move_str || strlen(move_str) < 4) return false;

    int from_file = move_str[0] - 'a', from_rank = move_str[1] - '1';
    int to_file   = move_str[2] - 'a', to_rank   = move_str[3] - '1';

    if (from_file < 0 || from_file > 7 || from_rank < 0 || from_rank > 7 ||
        to_file   < 0 || to_file   > 7 || to_rank   < 0 || to_rank   > 7) return false;

    uint64_t from_sq = 1ULL << (from_rank * 8 + from_file);
    uint64_t to_sq   = 1ULL << (to_rank   * 8 + to_file);

    // parse optional promotion piece
    piece_t promo = NULL_PIECE;
    if (move_str[4] != '\0') {
        switch (tolower((unsigned char)move_str[4])) {
            case 'q': promo = PIECE_QUEEN;  break;
            case 'r': promo = PIECE_ROOK;   break;
            case 'b': promo = PIECE_BISHOP; break;
            case 'n': promo = PIECE_KNIGHT; break;
        }
    }

    // find the matching legal move
    move_collection_t *legal = get_legal_moves_of_state(state);
    move_t *found = NULL;
    move_t *m;

    while ((m = pop_collection_head(legal)) != NULL) {
        if (get_move_from_square(m) != from_sq || get_move_to_square(m) != to_sq) {
            free_move(m);
            continue;
        }
        const flags_t *f = get_move_flags(m);

        if (promo != NULL_PIECE) {
            // match exactly when caller specified a promo piece
            if (f->promotion_piece == promo) {
                if (found) free_move(found);
                found = m;
            } else {
                free_move(m);
            }
        } else {
            // no promo specified: take first match, prefer queen if it's a promotion
            if (!found || f->promotion_piece == PIECE_QUEEN) {
                if (found) free_move(found);
                found = m;
            } else {
                free_move(m);
            }
        }
    }
    free_move_collection(legal);

    if (!found) return false;
    play_move(state, found);
    free_move(found);
    return true;
}
