/**
 * @file MoveCollection.h
 * @brief This file contains the declarations of the functions and data structures used for handling collections of chess moves.
 * 
 * @details The move_collection_t structure represents a collection of chess moves.
 * 
 * @version 1.0.0
 * @author Martin Newbound
 * @date 2024
 * 
 * @note License:
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef MOVE_COLLECTION_H
#define MOVE_COLLECTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "Move.h"


// Represents a collection of chess moves
typedef struct move_collection move_collection_t;

/**
 * Creates a new move collection.
 * 
 * @return A pointer to the new move collection.
 */
move_collection_t *new_move_collection();

/**
 * Frees a move collection.
 * 
 * @param collection The move collection to free.
 */
void free_move_collection(move_collection_t *collection);

/**
 * Pushes a move to a move collection.
 * 
 * @param move The move to push.
 * @param collection The move collection to push the move to.
 */
void push_move_to_collection(const move_t *move, move_collection_t *collection);

/**
 * Pops the head move from a move collection.
 * 
 * @param collection The move collection to pop the head move from.
 * @return A pointer to the popped move.
 */
move_t* pop_collection_head(move_collection_t *collection);

#ifdef __cplusplus
}
#endif

#endif // MOVE_COLLECTION_H