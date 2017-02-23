#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mqueue.h>


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
    char vector_name[64]; // Name of the vector we want to operate on.
    char response_queue[16]; // Name of the queue server will use to reponse.
    uint32_t index; // Index of the distributed vector.
    int vector_value; // Value of the vector at 'index'.
    int8_t error; // Check if the operations produce errors.
} msg_t;


int main(int argc, char **argv) {
    // Set the attributes for server's queue.
    struct mq_attr srv_attr;
    srv_attr.mq_maxmsg = MAX_QUEUE_SIZE;
    srv_attr.mq_msgsize = sizeof(msg_t);

    // Create server's queue.
    if ((server_queue = mq_open(srv_queue_name, O_CREAT|O_RDONLY, 0777,
        &srv_attr)) == -1) {
        perror("[ERR] Srv queue creation");
        exit (-1);
    }

    // Start listening.
    while(1) {
        msg_t new_msg;
        mqd_t client_queue;
        if (mq_receive(server_queue, (char *) &new_msg, sizeof(msg_t), 0) == -1) {
            perror("[ERR] Message reception");
            exit(-1);
        }

        new_msg.error = -1;
        if((client_queue = mq_open(new_msg.response_queue, O_WRONLY)) == -1) {
            perror("[ERR] Open client reponse queue");
        } else if (mq_send(client_queue, (const char *) &new_msg, sizeof(msg_t), 0) == -1) {
            perror("[ERR] Send reponse to client");
        }
    }

    return 0;
}
