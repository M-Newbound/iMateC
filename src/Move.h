#ifndef MOVE_H

#include <stdint.h>
#include "GameState.h"

typedef struct Move move_t;

void populate_move(move_t *move, uint64_t fromMask, uint64_t toMask, color_key_t color);

void add_promotion_flag(move_t *move, piece_key_t chosenPiece);
void add_castling_flag(move_t *move, castle_key_t castle);
void add_double_pawn_push_flag(move_t *move);
void add_en_passant_flag(move_t *move);
void add_capture_flag(move_t *move);

void apply_move(const move_t *move, game_state_t *state);

#endif // MOVE_H