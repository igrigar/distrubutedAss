#ifndef SERVER_
#define SERVER_

#include <pthread.h>

/******************************************************************************
 *                              Global Definitions                            *
 ******************************************************************************/

// Name of the queue for communicating with the server.
const char rv_queue_name[16] = "/dist_vec_queue";

// Queue descriptors.
mqd_t server_queue = NULL;

// Define the maximum lenght of a queue.
const uint8_t MAX_QUEUE_SIZE = 15;

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
 *                            Structure Definitions                           *
 ******************************************************************************/
// Services provided by the server.
typedef enum {INIT, SET, GET, KILL} msg_code;

// Struct that defines a message of the queue.
typedef struct {
    msg_code service; // Operation requested to the server.
    char vector_name[16]; // Name of the vector we want to operate on.
    char response_queue[16]; // Name of the queue server will use to reponse.
    uint32_t index; // Index of the distributed vector.
    int vector_value; // Value of the vector at 'index'.
    int8_t error; // Check if the operations produce errors.
} msg_t;

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
