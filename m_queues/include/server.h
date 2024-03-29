#ifndef SERVER_
#define SERVER_

#include <pthread.h>

#include "msg.h"

/******************************************************************************
 *                              Struct deffinitions                           *
 ******************************************************************************/

// Struct representing a distributed vector.
typedef struct {
    char name[MAX_NAME_LENGTH];
    uint32_t size;
    int *vector;
} d_vec_t;

/******************************************************************************
 *                              Global Definitions                            *
 ******************************************************************************/

// Name of the queue for communicating with the server.
//const char srv_queue_name[16] = "/dist_vec_queue";

// Queue descriptors.
mqd_t server_queue = NULL;

// Distributed vector structures.
const uint8_t NUM_VECTORS = 4;
const uint8_t VECTOR_NAME_LENGTH = 16;

// Index with the names of all the created vectors.
char **vector_index;
int **vector;

// Concurrency stuff (Mutex, cond, ...)
pthread_mutex_t index_l; // Control 'vector_index'.
pthread_cond_t index_u;
uint8_t index_c = 0;

pthread_mutex_t vector_l; // Control 'vector'.
pthread_cond_t vector_u;
uint8_t vector_c = 0;

/******************************************************************************
 *                             Function Prototypes                            *
 ******************************************************************************/

void init_vector(msg_t *msg);
void set_vector(msg_t *msg);
void get_vector(msg_t *msg);
void kill_vector(msg_t *msg);
int8_t find_index(char *name);
void send(msg_t *msg);

#endif
