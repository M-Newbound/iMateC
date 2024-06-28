#ifndef  GAME_STATE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    WHITE           = 0,
    BLACK           = 1
} color_t;


typedef enum {
    PIECE_PAWN      = 0,
    PIECE_KNIGHT    = 1,
    PIECE_BISHOP    = 2,
    PIECE_ROOK      = 3,
    PIECE_QUEEN     = 4,
    PIECE_KING      = 5
} piece_t;


typedef enum {
    CASTLE_KINGSIDE_WHITE   = 0,
    CASTLE_QUEENSIDE_WHITE  = 1,
    CASTLE_KINGSIDE_BLACK   = 2,
    CASTLE_QUEENSIDE_BLACK  = 3
} castle_t;


/**
 * @brief Forward declaration of the state struct
 * 
 * The inner workings of the state struct are hidden as
 * they are not relevant to the public interface of this module
 */
typedef struct state state_t;


/**
 * @brief Initializes the game state with an empty board.
 *
 * @param state     Pointer to the game state.
 */
void reset_state(state_t *state);


/**
 * @brief Copies the game state from one struct to another.
 *
 * This function is used to create a duplicate of a game state. This is useful
 * when you want to make changes to the game state, but also need to keep
 * the original state unchanged for comparison or rollback purposes.
 *
 * @param fromState     Pointer to the source game state.
 * @param toState       Pointer to the destination game state.
 */
void copy_state(const state_t *fromState, state_t *toState);


/**
 * @brief Retrieves the castling right for a specific type of castling.
 *
 * In chess, castling is a special move that involves the player's king and one of their rooks.
 * There are two types of castling: kingside and queenside. This function retrieves the castling
 * right for a specific type of castling.
 *
 * This function is needed to check if a player is allowed to castle at a given point in the game.
 * Castling rights can be lost if the king or the rook has moved, or if the king is in check.
 *
 * @param state     Pointer to the game state.
 * @param castle    The type of castling to check the right for.
 * 
 * @return          true if the player has the right to castle, false otherwise.
 */
bool state_can_castle(const state_t *state, castle_t castle);


/**
 * @brief Retrieves the bitboard representing the positions of all pieces of a specific color.
 *
 * A bitboard is a data structure used in computer systems that play board games, where each bit corresponds
 * to a game board space or piece. This function retrieves the bitboard for a specific color.
 *
 * @param state     Pointer to the game state.
 * @param color     The color of the pieces to get the bitboard for.
 * 
 * @return          The bitboard for the specified color.
 */
uint64_t states_color_bitboard(const state_t *state, color_t color);


/**
 * @brief Retrieves the bitboard representing the positions of a specific type of piece of a specific color.
 *
 * This function retrieves the bitboard for a specific type of piece of a specific color.
 *
 * @param state     Pointer to the game state.
 * @param piece     The type of piece to get the bitboard for.
 * @param color     The color of the pieces to get the bitboard for.
 * 
 * @return          The bitboard for the specified piece and color.
 */
uint64_t states_piece_bitboard(const state_t *state, piece_t piece, color_t color);


/**
 * @brief Sets the bitboard for a specific type of piece of a specific color.
 *
 * This function is used to update the game state when a piece is moved or removed from the game.
 *
 * @param state         Pointer to the game state.
 * @param piece         The type of piece to set the bitboard for.
 * @param color         The color of the pieces to set the bitboard for.
 * @param bitboard      The new bitboard for the specified piece and color.
 */
void set_states_piece_bitboard(state_t *state, piece_t piece, color_t color, uint64_t bitboard);


#endif // GAME_STATE_H