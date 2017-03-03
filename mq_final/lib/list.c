#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

void enqueue(node_t *head, vector_t vector) {
    node_t *current = head;

    while (current->next != NULL)
        current = current->next;

    current->next = (node_t *) malloc(sizeof(node_t));
    current->next->next = NULL;
    current->next->val = vector;
}

void remove_node(node_t **head, uint32_t idx) {
    node_t *current = *head;
    node_t *tmp = NULL;

    uint32_t i;

    if (idx == 0) { // Remove head.
    }

    for (i = 0; i < (idx - 1); ++i)
        if (current->next == NULL) // Index bigger than list size.
            return;
        else
            current = current->next;

    tmp = current->next;
    current->next = tmp->next;
    free(tmp->val.vector);
    free(tmp);
}

int32_t find_name(node_t *head, char *name) {
    node_t *current = head;
    int32_t idx = 0;

    while (current->next != NULL)
        if (!strcmp(current->val.name, name))
            return idx;
        else {
            current = current->next;
            ++idx;
        }

    if (!strcmp(current->val.name, name))
        return idx;

    return -1;
}

int8_t set_value(node_t *head, uint32_t idx, uint32_t v_idx, int val) {
    node_t *current = head;
    uint32_t i;

    for (i = 0; i < idx; ++i)
        if (current->next == NULL)
            return -1;
        else
            current = current->next;

    if (v_idx >= current->val.size)
        return -1;

    current->val.vector[v_idx] = val;
    return 0;
}

int8_t get_value(node_t *head, uint32_t idx, uint32_t v_idx, int *val) {
    node_t *current = head;
    uint32_t i;

    for (i = 0; i < idx; ++i)
        if (current == NULL)
            return -1;
        else
            current = current->next;

    if (v_idx >= current->val.size) 
        return -1;

    *val = current->val.vector[v_idx];
    return 0;
}
