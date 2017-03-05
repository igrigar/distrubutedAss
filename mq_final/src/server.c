#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "list.h"
#include "msg.h"

mqd_t server_q = NULL; // Server's queue descriptor.
node_t *v_list = NULL; // List of vectors.

// Protecting the list.
pthread_mutex_t list_lck;
pthread_cond_t list_ulck;
uint32_t list_workers = 0;

// Function prototypes.
void init_vector(msg_t *msg);
void v_list_lck();
void v_list_ulck();
void send(msg_t *msg);
void set_vector(msg_t *msg);
void get_vector(msg_t *msg);
void kill_vector(msg_t *msg);

int main() {

    // Setting the attributes for the queue.
    struct mq_attr srv_attr;
    srv_attr.mq_maxmsg = MAX_QUEUE_SIZE;
    srv_attr.mq_msgsize = sizeof(msg_t);

    // Server's queue creation.
    if ((server_q = mq_open(srv_queue_name, O_CREAT|O_RDONLY, 0777,
            &srv_attr)) == -1) {
        perror("[ERR] Srv queue creation");
        exit (-1);
    }

    // Initializing concurrency stuff.
    if (pthread_mutex_init(&list_lck, NULL) != 0 ) {
        perror("[ERR] Index lock.");
        exit(-1);
    }
    if (pthread_cond_init(&list_ulck, NULL) != 0) {
        perror("[ERR] Vector lock.");
        exit(-1);
    }

    // Start listening.
    while(1) {
        msg_t msg; // Received message.
        pthread_t thread; // Thread to be launched.

        // Get the received message.
        if (mq_receive(server_q, (char *) &msg, sizeof(msg_t), 0) == -1) {
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
            default: send(&msg);
        }
    }

    if (mq_close(server_q) == -1)
        perror("[ERR] Close response queue.");

    return 0;
}

/*
 * @Brief: handler function for INIT request. If the name is not in use, enqueue
 * the vector in the list.
 * @Param: *msg, pointer to the incoming message.
 */
void init_vector(msg_t *msg) {
    msg_t m = *msg;
    int32_t idx;

    if (m.index == 0) { // It does not make sense to have size 0.
        send(&m);
        return;
    }

    // Create the vector entry in case it can be inserted.
    vector_t n_vector;
    memcpy(n_vector.name, m.vector_name, NAME_SIZE);
    n_vector.size = m.index;
    n_vector.vector = (int *) malloc(m.index * sizeof(int));

    v_list_lck(); // Lock the list. 
    if (v_list == NULL) { // List empty v_list
        v_list = (node_t *) malloc(sizeof(node_t)); 
        v_list->val = n_vector;
        v_list->next = NULL; 
        m.error = 1; // Operation successfull.  
    } else if ((idx = find_name(v_list, m.vector_name)) == -1) {
        enqueue(v_list, n_vector);
        m.error = 1; // Operation successfull.  
    } else if (get_vector_size(v_list, (uint32_t) idx) == m.index)
        m.error = 0; 
    v_list_ulck(); // Release the list.

    send(&m);
}

/*
 * @Brief: handler function for SET request. Find the requested vector and
 *  perform set operation, if possible.
 * @Param: *msg, pointer to the incoming message.
 */
void set_vector(msg_t *msg) {
    msg_t m = *msg;

    v_list_lck();
    int32_t idx = find_name(v_list, m.vector_name);
    v_list_ulck();

    v_list_lck();
    m.error = set_value(v_list, (uint32_t) idx, m.index, m.vector_value);
    v_list_ulck();

    send(&m);
}

/*
 * @Brief: handler function for GET request. Find the requested vector and
 *  perform get operation, if possible.
 * @Param: *msg, pointer to the incoming message.
 */
void get_vector(msg_t *msg) {
    msg_t m = *msg;

    v_list_lck();
    int32_t idx = find_name(v_list, m.vector_name);
    v_list_ulck();

    v_list_lck();
    m.error = get_value(v_list, (uint32_t) idx, m.index, &m.vector_value);
    v_list_ulck();

    send(&m);
}

/*
 * @Brief: handler function for KILL request. Find the requested vector and,
 *  liberatie it's memory and it's index entry.
 * @Param: *msg, pointer to the incoming message.
 */
void kill_vector(msg_t *msg) {
    msg_t m = *msg;

    v_list_lck();
    int32_t idx = find_name(v_list, m.vector_name);
    v_list_ulck();

    v_list_lck();
    remove_node(&v_list, (uint32_t) idx);
    v_list_ulck();

    if (idx >= 0) m.error = 1;

    send(&m);
}

/*
 * @Brief: procedure to lock the list of vectors.
 */
void v_list_lck() {
    pthread_mutex_lock(&list_lck);
    while (list_workers)
        pthread_cond_wait(&list_ulck, &list_lck);
    ++list_workers;
}

/*
 * @Brief: procedure to unlock the list of vectors.
 */
void v_list_ulck() {
    --list_workers;
    pthread_cond_broadcast(&list_ulck);
    pthread_mutex_unlock(&list_lck);
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
    else if (mq_close(cli_queue) == -1)
        perror("[ERR] Close response queue.");
}
