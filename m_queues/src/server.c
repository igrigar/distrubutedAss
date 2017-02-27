#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <mqueue.h>

#include "server.h"
#include "msg.h"

int main() {
    // Set the attributes for server's queue.
    struct mq_attr srv_attr;
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

    // Initializing concurrency stuff.
    if (pthread_mutex_init(&index_l, NULL) != 0 ) {
        perror("[ERR] Index lock.");
        exit(-1);
    }
    if (pthread_mutex_init(&vector_l, NULL) != 0) {
        perror("[ERR] Vector lock.");
        exit(-1);
    }

    if (pthread_cond_init(&index_u, NULL) != 0) {
        perror("[ERR] Index cond.");
        exit(-1);
    }
    if (pthread_cond_init(&vector_u, NULL) != 0) {
        perror("[ERR] Vector cond.");
        exit(-1);
    }

    // Start listening.
    while(1) {
        msg_t msg; // Received message.
        pthread_t thread; // Thread to be launched.

        // Get the received message.
        if (mq_receive(server_queue, (char *) &msg, sizeof(msg_t), 0) == -1) {
            perror("[ERR] Message reception");
            exit(-1);
        }

        msg.error = -1; // We assume that the operation is wrong.

        // Concurrent request handling.
        switch(msg.service) {
            case INIT:
                if (pthread_create(&thread, NULL, (void *) init_vector, &msg)
                    == -1)
                    printf("Error al crear los threads\n");
                else
                    pthread_detach(thread);
                break;
            case GET:
                if (pthread_create(&thread, NULL, (void *) get_vector, &msg)
                    == -1)
                    printf("Error al crear los threads\n");
                else
                    pthread_detach(thread);
                break;
            case SET:
                if (pthread_create(&thread, NULL, (void *) set_vector, &msg)
                    == -1)
                    printf("Error al crear los threads\n");
                else
                    pthread_detach(thread);
                break;
            case KILL:
                if (pthread_create(&thread, NULL, (void *) kill_vector, &msg)
                    == -1)
                    printf("Error al crear los threads\n");
                else
                    pthread_detach(thread);
                break;
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

    // Look for an empty entry in index and check if vector name already in
    // index.
    for (; i < NUM_VECTORS; ++i) {
        pthread_mutex_lock(&index_l); // Lock to access shared structure.
        while(index_c) { // If index consumers/producers working, wait.
            pthread_cond_wait(&index_u, &index_l);
        }
        index_c = 1; // Indicate that we are working on shared structure.

        if(!vector_index[i])
            free_index = i;
        else if (!strcmp(vector_index[i], message.vector_name)) {
            name_in_use = 1;
            printf("[INIT] '%s' already in use.\n", message.vector_name);
        }

        // Unlock and awake.
        index_c = 0;
        pthread_cond_broadcast(&index_u);
        pthread_mutex_unlock(&index_l);
    }

    // Creation of index entry and vector.
    if (!name_in_use && free_index >= 0) {
        // Creating entry for the vector index.
        pthread_mutex_lock(&index_l); // Lock to access shared structure.
        while(index_c) { // If index consumers/producers working, wait.
            pthread_cond_wait(&index_u, &index_l);
        }
        index_c = 1; // Indicate that we are working on shared structure.

        vector_index[free_index] = (char *) calloc(strlen(message.vector_name),
            sizeof(char));
        memcpy(vector_index[free_index], message.vector_name,
            strlen(message.vector_name));

        // Unlock and awake.
        index_c = 0;
        pthread_cond_broadcast(&index_u);
        pthread_mutex_unlock(&index_l);

        // Creating entry for the vector itself.
        pthread_mutex_lock(&vector_l); // Lock to access shared structure.
        while(vector_c) { // If index consumers/producers working, wait.
            pthread_cond_wait(&vector_u, &vector_l);
        }
        vector_c = 1; // Indicate that we are working on shared structure.

        vector[free_index] = (int *) calloc((size_t) message.vector_value,
            sizeof(int));
        bzero(vector[free_index], (size_t) message.vector_value);

        // Unlock and awake.
        vector_c = 0;
        pthread_cond_broadcast(&vector_u);
        pthread_mutex_unlock(&vector_l);

        message.error = 0;

        printf("[INIT] '%s' at %d\n", message.vector_name, free_index);
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
        pthread_mutex_lock(&vector_l); // Lock to access shared structure.
        while(vector_c) { // If index consumers/producers working, wait.
            pthread_cond_wait(&vector_u, &vector_l);
        }
        vector_c = 1; // Indicate that we are working on shared structure.

        vector[index][message.index] = message.vector_value;

        // Unlock and awake.
        vector_c = 0;
        pthread_cond_broadcast(&vector_u);
        pthread_mutex_unlock(&vector_l);

        message.error = 0;
        printf("[SET] %s[%d] <- %d.\n", message.vector_name, message.index,
            message.vector_value);
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
        pthread_mutex_lock(&vector_l); // Lock to access shared structure.
        while(vector_c) { // If index consumers/producers working, wait.
            pthread_cond_wait(&vector_u, &vector_l);
        }
        vector_c = 1; // Indicate that we are working on shared structure.

        message.vector_value = vector[index][message.index];

        // Unlock and awake.
        vector_c = 0;
        pthread_cond_broadcast(&vector_u);
        pthread_mutex_unlock(&vector_l);

        message.error = 0;
        printf("[GET] %s[%d] -> %d.\n", message.vector_name, message.index,
            message.vector_value);
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
        pthread_mutex_lock(&vector_l); // Lock to access shared structure.
        while(vector_c) { // If index consumers/producers working, wait.
            pthread_cond_wait(&vector_u, &vector_l);
        }
        vector_c = 1; // Indicate that we are working on shared structure.

        free(vector[index]);

        // Unlock and awake.
        vector_c = 0;
        pthread_cond_broadcast(&vector_u);
        pthread_mutex_unlock(&vector_l);
        pthread_mutex_lock(&index_l); // Lock to access shared structure.

        while(index_c) { // If index consumers/producers working, wait.
            pthread_cond_wait(&index_u, &index_l);
        }
        index_c = 1; // Indicate that we are working on shared structure.

        free(vector_index[index]);
        vector_index[index] = NULL;

        // Unlock and awake.
        index_c = 0;
        pthread_cond_broadcast(&index_u);
        pthread_mutex_unlock(&index_l);

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

    for (; i < NUM_VECTORS; ++i) if(vector_index[i]) {
        pthread_mutex_lock(&index_l); // Lock to access shared structure.
        while(index_c) { // If index consumers/producers working, wait.
            pthread_cond_wait(&index_u, &index_l);
        }
        index_c = 1; // Indicate that we are working on shared structure.
        if (!strcmp(vector_index[i], name)) {
            // Unlock and awake.
            index_c = 0;
            pthread_cond_broadcast(&index_u);
            pthread_mutex_unlock(&index_l);

            return i;
        }

        // Unlock and awake.
        index_c = 0;
        pthread_cond_broadcast(&index_u);
        pthread_mutex_unlock(&index_l);
    }

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
