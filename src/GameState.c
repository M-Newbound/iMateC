#include "GameState.h"

#include <string.h>


struct en_passant_target_square {
    uint64_t square;
    bool exists;
};


struct state {
    uint64_t bitboards[2][6];   // 2 colors, 6 piece types
    bool castlingRights[4];     // 4 castling rights
   
    struct en_passant_target_square enPassantTargetSquare;
};


void reset_state(state_t *state) {
    memset(state->bitboards,        0,      sizeof(state->bitboards)     );    
    memset(state->castlingRights,   true,   sizeof(state->castlingRights));
    
    state->enPassantTargetSquare.exists = false;
    state->enPassantTargetSquare.square = 0;
}


void copy_state(const state_t *fromState, state_t *toState) {
    memcpy(toState->bitboards,      fromState->bitboards,       sizeof(toState->bitboards)      );
    memcpy(toState->castlingRights, fromState->castlingRights,  sizeof(toState->castlingRights) );
    
    toState->enPassantTargetSquare = fromState->enPassantTargetSquare;
}


uint64_t states_color_bitboard(const state_t *state, color_t color) {
    uint64_t colorBitboard = 0;
    for (size_t i = PIECE_PAWN; i <= PIECE_KING; i++) {
        colorBitboard |= state->bitboards[color][i];
    }
    return colorBitboard;
}

uint64_t states_piece_bitboard(const state_t *state, piece_t piece, color_t color) {
    return state->bitboards[color][piece];
}


void set_states_piece_bitboard(state_t *state, piece_t piece, color_t color, uint64_t bitboard) {
    state->bitboards[color][piece] = bitboard;
}


bool state_can_castle(const state_t *state, castle_t castle) {
    return state->castlingRights[castle];
}
