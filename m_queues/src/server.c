#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <mqueue.h>

/******************************************************************************
 *             Constant Stuff (May be inserted in a header file)              *
 ******************************************************************************/
// Name of the queue for communicating with the server.
const char rv_queue_name[16] = "/dist_vec_queue";

// Queue descriptors.
mqd_t server_queue = NULL;

// Define the maximum lenght of a queue.
const uint8_t MAX_QUEUE_SIZE = 15;

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

// Distributed vector structures.
const uint8_t NUM_VECTORS = 4;
const uint8_t VECTOR_NAME_LENGTH = 16;

// Index with the names of all the created vectors.
char **vector_index;
int **vector;

// Function prototypes.
void init_vector(msg_t *msg);
void set_vector(msg_t *msg);
void get_vector(msg_t *msg);
void kill_vector(msg_t *msg);
int8_t find_index(char *name);
void send(msg_t *msg);

/******************************************************************************
 *        End of Constant Stuff (May be inserted in a header file)            *
 ******************************************************************************/

int main(int argc, char **argv) {
    // Set the attributes for server's queue.
    struct mq_attr srv_attr;
    srv_attr.mq_maxmsg = MAX_QUEUE_SIZE;
    srv_attr.mq_msgsize = sizeof(msg_t);

    // Wipe memory section of vector index.
    vector_index = (char **) malloc(NUM_VECTORS * sizeof(char *));
    vector = (int **) malloc(NUM_VECTORS * sizeof(int *));

    // Create server's queue.
    if ((server_queue = mq_open(rv_queue_name, O_CREAT|O_RDONLY, 0777,
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
            case GET: get_vector(&msg); break;
            case SET: set_vector(&msg); break;
            case KILL: kill_vector(&msg); break;
            default: break;
        }
    }

    return 0;
}

/*
 * @Brief: handler function for INIT request. Create an entry, if possible, in
 * vector_index and create the vector itself.
 * @Param: *msg, pointer to the incoming message.
 */
void init_vector(msg_t *msg) {
    msg_t message = *msg;
    int8_t i = 0;
    int8_t name_in_use = 0; // Used to check repeated vector names.
    int8_t free_index = -1; // Used in the search of free indices.

    for (; i < NUM_VECTORS; ++i) {
        if(!vector_index[i])
            free_index = i;
        else if (!strcmp(vector_index[i], message.vector_name)) {
            name_in_use = 1;
            printf("[INIT] '%s' already in use.\n", message.vector_name);
        }
    }

    if (!name_in_use && free_index >= 0) {
        // Creating entry for the vector index.
        char *name = (char *) calloc(strlen(message.vector_name), sizeof(char));
        memcpy(name, message.vector_name, strlen(message.vector_name));
        vector_index[free_index] = name;

        // Creating entry for the vector itself.
        int *vector_n = (int *) calloc((size_t) message.vector_value, sizeof(int));
        bzero(vector_n, (size_t) message.vector_value);
        vector[free_index] = vector_n;

        message.error = 0;

        printf("[INIT] '%s' at %d\n", name, free_index);
    } else if (free_index == -1)
        printf("[INIT] Index full; '%s' dropped.\n", message.vector_name);

    send(&message);
}

/*
 * @Brief: handler function for SET request. Find the requested vector and
 *  perform set operation, if possible.
 * @Param: *msg, pointer to the incoming message.
 */
void set_vector(msg_t *msg) {
    msg_t message = *msg;
    int8_t index = -1;

    if ((index = find_index(message.vector_name)) >= 0) {
        vector[index][message.index] = message.vector_value;
        message.error = 0;
        printf("[SET] %s[%d] <- %d.\n", message.vector_name, message.index, message.vector_value);
    }

    send(&message);
}

/*
 * @Brief: handler function for gET request. Find the requested vector and
 *  perform get operation, if possible.
 * @Param: *msg, pointer to the incoming message.
 */
void get_vector(msg_t *msg) {
    msg_t message = *msg;
    int8_t index = -1;

    if ((index = find_index(message.vector_name)) >= 0) {
        message.vector_value = vector[index][message.index];
        message.error = 0;
        printf("[GET] %s[%d] -> %d.\n", message.vector_name, message.index, message.vector_value);
    }

    send(&message);
}

/*
 * @Brief: handler function for KILL request. Find the requested vector and,
 *  liberatie it's memory and it's index entry.
 * @Param: *msg, pointer to the incoming message.
 */
void kill_vector(msg_t *msg) {
    msg_t message = *msg;
    int8_t index = -1;

    if ((index = find_index(message.vector_name)) >= 0) {
        free(vector[index]);
        free(vector_index[index]);
        message.error = 0;

        printf("[KILL] '%s' deleted.\n", message.vector_name);
    }

    send(&message);
}

/*
 * @Brief: look-up a vector name in the index.
 * @Param: name; name of the vector we are looking for.
 * @Return: index of the vector if it exist, -1 otherwise.
 */
int8_t find_index(char *name) {
    int8_t i = 0;

    for (; i < NUM_VECTORS; ++i) if(vector_index[i])
        if (!strcmp(vector_index[i], name))
            return i;

    printf("[IDX] '%s' not found in the index.\n", name);
    return -1;
}

/*
 * @Brief: send response message to a client.
 * @Param: msg; message to be sent.
 */
void send(msg_t *msg) {
    mqd_t cli_queue;

    if ((cli_queue = mq_open(msg->response_queue, O_WRONLY)) == -1)
        perror("[ERR] Open client reponse queue");
    else if (mq_send(cli_queue, (const char *) msg, sizeof(msg_t), 0) == -1)
        perror("[ERR] Send reponse to client");
}
