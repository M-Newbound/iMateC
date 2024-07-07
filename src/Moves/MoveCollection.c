/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "MoveCollection.h"
#include <stdlib.h>


struct move_collection_node {
    const move_t *move;
    struct move_collection_node *next;
};


struct move_collection {
    struct move_collection_node *head;
};

move_collection_t *new_move_collection() {
    move_collection_t *collection = malloc(sizeof(move_collection_t));
    collection->head = NULL;
    return collection;
}

void free_move_collection(move_collection_t *collection) {
    struct move_collection_node *current = collection->head;
    while (current != NULL) {
        struct move_collection_node *next = current->next;
        free_move(current->move);
        free(current);
        current = next;
    }

    collection->head = NULL;
}


void push_move_to_collection(const move_t *move, move_collection_t *collection) {
    struct move_collection_node *new_node = malloc(sizeof(struct move_collection_node));
    new_node->move = move;
    new_node->next = collection->head;
    collection->head = new_node;
}

move_t *pop_collection_head(move_collection_t *collection) {
    struct move_collection_node *head = collection->head;
    if (head == NULL) return NULL;

    move_t *move = head->move;
    struct move_collection_node *next = head->next;

    free(head);
    collection->head = next;
    return move;
}
