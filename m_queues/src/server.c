#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <mqueue.h>

/******************************************************************************
 *             Constant Stuff (May be inserted in a header file)              *
 ******************************************************************************/
// Name of the queue for communicating with the server.
const char srv_queue_name[16] = "/dist_vec_queue";

// Queue descriptors.
mqd_t server_queue = NULL;

// Define the maximum lenght of a queue.
const uint8_t MAX_QUEUE_SIZE = 15;

// Services provided by the server.
typedef enum {INIT, SET, GET, KILL} msg_code;

// Struct that defines a message of the queue.
typedef struct {
    msg_code service; // Operation requested to the server.
    char vector_name[32]; // Name of the vector we want to operate on.
    char response_queue[16]; // Name of the queue server will use to reponse.
    uint32_t index; // Index of the distributed vector.
    int vector_value; // Value of the vector at 'index'.
    int8_t error; // Check if the operations produce errors.
} msg_t;

// Distributed vector structures.
const uint8_t NUM_VECTORS = 15;
const uint8_t VECTOR_NAME_LENGTH = 32;

// Index with the names of all the created vectors.
char **vector_index;
int **vector;

// Function prototypes.
void init_vector(msg_t *msg);
void set_vector(msg_t *msg);

/******************************************************************************
 *        End of Constant Stuff (May be inserted in a header file)            *
 ******************************************************************************/

int main(int argc, char **argv) {
    // Set the attributes for server's queue.
    struct mq_attr srv_attr;
    uint8_t i;
    srv_attr.mq_maxmsg = MAX_QUEUE_SIZE;
    srv_attr.mq_msgsize = sizeof(msg_t);

    // Wipe memory section of vector index.
    vector_index = (char **) malloc(NUM_VECTORS * sizeof(char *));
    vector = (int **) malloc(NUM_VECTORS * sizeof(int *));

    // Create server's queue.
    if ((server_queue = mq_open(srv_queue_name, O_CREAT|O_RDONLY, 0777,
        &srv_attr)) == -1) {
        perror("[ERR] Srv queue creation");
        exit (-1);
    }

    // Start listening.
    while(1) {
        msg_t msg; // Received message.

        // Get the received message.
        if (mq_receive(server_queue, (char *) &msg, sizeof(msg_t), 0) == -1) {
            perror("[ERR] Message reception");
            exit(-1);
        }

        msg.error = -1; // We assume that the operation is wrong.

        switch(msg.service) {
            case INIT: init_vector(&msg); break;
            case GET: break;
            case SET: set_vector(&msg); break;
            case KILL: break;
            default: break;
        }

        for (i = 0; i < NUM_VECTORS; ++i) {
            if (vector_index[i]) {
                printf("Vector: %s at %d.\n", vector_index[i], i);
            }
        }
    }

    return 0;
}

void init_vector(msg_t *msg) {
    msg_t message = *msg;
    mqd_t cli_queue;
    uint8_t i = 0;
    uint8_t name_in_use = 0; // Used to check repeated vector names.
    int8_t free_index = -1; // Used in the search of free indices.

    for (i; i < NUM_VECTORS; ++i) {
        if (!vector_index[i])
            free_index = i;
        else if (!strcmp(vector_index[i], message.vector_name))
            name_in_use = 1;
    }

    if (!name_in_use && free_index >= 0) {
        // Creating entry for the vector index.
        char *name = (char *) calloc(strlen(message.vector_name), sizeof(char));
        memcpy(name, message.vector_name, strlen(message.vector_name));
        vector_index[free_index] = name;

        // Creating entry for the vector itself.
        int *vector_n = (int *) calloc(message.vector_value, sizeof(int));
        bzero(vector_n, message.vector_value);
        vector[free_index] = vector_n;

        message.error = 0;
    }

    if ((cli_queue = mq_open(message.response_queue, O_WRONLY)) == -1)
        perror("[ERR] Open client reponse queue");
    else if (mq_send(cli_queue, (const char *) &message, 
        sizeof(msg_t), 0) == -1) {
        perror("[ERR] Send reponse to client");
    }
}

void set_vector(msg_t *msg) {
    msg_t message = *msg;
    mqd_t cli_queue;
    uint8_t i = 0;

    for (i; i < NUM_VECTORS; ++i) {
        if(vector_index[i])
            if (!strcmp(vector_index[i], message.vector_name)) {
    //            if (sizeof(*vector)/sizeof(*vector) > message.index) {
    //                printf("Index on bound\n");
                    vector[i][message.index] = message.vector_value;
                    message.error = 0;
    //            }
                break;
            }
    }

    if ((cli_queue = mq_open(message.response_queue, O_WRONLY)) == -1)
        perror("[ERR] Open client reponse queue");
    else if (mq_send(cli_queue, (const char *) &message, 
        sizeof(msg_t), 0) == -1) {
        perror("[ERR] Send reponse to client");
    }

    printf("%d\n", vector[i][message.index]); 
}
