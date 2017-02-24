#ifndef ARRAY_
#define ARRAY_

#include <stdint.h>

// Name of the queue for communicating with the server.
//const char srv_queue_name[16] = "/dist_vec_queue";

// Define the maximum lenght of a queue.
//const uint8_t MAX_QUEUE_SIZE = 15;

// Services provided by the server.
typedef enum {INIT, SET, GET, KILL} msg_code;

// Struct that defines a message of the queue.
typedef struct {
    msg_code service; // Operation requested to the server.
    char vector_name[32]; // Name of the vector we want to operate on.
    char reponse_queue[16]; // Name of the queue server will use to reponse.
    uint32_t index; // Index of the distributed vector.
    int vector_value; // Value of the vector at 'index'.
                      // Used to provide the size of the vector in INIT.
    int8_t error; // Check if the operations produce errors.
} msg_t;

int init(char *name, int n);
int set(char *name, int i, int value);
int get(char *name, int i, int *value);
int destroy(char *name);

#endif
