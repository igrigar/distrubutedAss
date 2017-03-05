#ifndef _VECTOR_LIST_
#define _VECTOR_LIST_

#include <stdint.h>

#define NAME_SIZE 16

typedef struct {
    char name[NAME_SIZE];
    uint32_t size;
    int *vector;
} vector_t;

typedef struct node {
    vector_t val;
    struct node *next;
} node_t;

void enqueue(node_t *head, vector_t vector);
void remove_node(node_t **head, uint32_t idx);
int32_t find_name(node_t *head, char *name);
int8_t set_value(node_t *head, uint32_t idx, uint32_t v_idx, int val);
int8_t get_value(node_t *head, uint32_t idx, uint32_t v_idx, int *val);
uint32_t get_vector_size(node_t *head, uint32_t idx);

#endif
