/* iMate -- Copyright (C) 2024 Martin Newbound */

/* A singly-linked list of chess moves. Ownership follows push/pop:
 * push transfers ownership to the collection, pop transfers it back to the caller. */

#ifndef MOVE_COLLECTION_H
#define MOVE_COLLECTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "Move.h"

typedef struct move_collection move_collection_t;

move_collection_t *new_move_collection(void);
void               free_move_collection(move_collection_t *collection);
void               push_move_to_collection(move_t *move, move_collection_t *collection);
move_t            *pop_collection_head(move_collection_t *collection);
bool               move_collection_is_empty(const move_collection_t *collection);

#ifdef __cplusplus
}
#endif

#endif /* MOVE_COLLECTION_H */
