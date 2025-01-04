/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "MoveCollection.h"
#include <stdlib.h>

struct move_collection_node {
    move_t *move;
    struct move_collection_node *next;
};

struct move_collection {
    struct move_collection_node *head;
};

move_collection_t *new_move_collection(void) {
    move_collection_t *collection = malloc(sizeof(move_collection_t));
    collection->head = NULL;
    return collection;
}

void free_move_collection(move_collection_t *collection) {
    struct move_collection_node *node = collection->head;
    while (node) {
        struct move_collection_node *next = node->next;
        free_move(node->move);
        free(node);
        node = next;
    }
    free(collection);
}

void push_move_to_collection(move_t *move, move_collection_t *collection) {
    struct move_collection_node *node = malloc(sizeof(struct move_collection_node));
    node->move       = move;
    node->next       = collection->head;
    collection->head = node;
}

move_t *pop_collection_head(move_collection_t *collection) {
    struct move_collection_node *head = collection->head;
    if (!head) return NULL;

    move_t *move = head->move;
    collection->head = head->next;
    free(head);
    return move;
}

bool move_collection_is_empty(const move_collection_t *collection) {
    return collection->head == NULL;
}
