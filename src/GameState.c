#include "GameState.h"

#include <string.h>


struct EnPassantTargetSquare {
    bool exists;
    uint64_t square;
};


struct GameState {
    uint64_t bitboards[2][6];   // 2 colors, 6 piece types
    bool castlingRights[4];     // 4 castling rights
   
    struct EnPassantTargetSquare enPassantTargetSquare;
};


void copy_game_state(const game_state_t *fromState, game_state_t *toState) {
    memcpy(toState->bitboards,      fromState->bitboards,       sizeof(toState->bitboards)      );
    memcpy(toState->castlingRights, fromState->castlingRights,  sizeof(toState->castlingRights) );
    
    toState->enPassantTargetSquare = fromState->enPassantTargetSquare;
}


uint64_t get_color_bitboard(const game_state_t *state, color_key_t color) {
    uint64_t colorBitboard = 0;
    for (int i = PIECE_PAWN; i <= PIECE_KING; i++) {
        colorBitboard |= state->bitboards[color][i];
    }
    return colorBitboard;
}

uint64_t get_piece_bitboard(const game_state_t *state, piece_key_t piece, color_key_t color) {
    return state->bitboards[color][piece];
}


void set_piece_bitboard(game_state_t *state, piece_key_t piece, color_key_t color, uint64_t bitboard) {
    state->bitboards[color][piece] = bitboard;
}


bool get_castling_right(const game_state_t *state, castle_key_t castle) {
    return state->castlingRights[castle];
}
