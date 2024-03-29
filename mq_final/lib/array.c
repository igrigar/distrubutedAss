#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mqueue.h>

#include "array.h"
#include "msg.h"

// Queue descriptors.
mqd_t server_queue = NULL;
mqd_t client_queue = NULL;

// Client queue name.
char *cli_queue_name;

/*
 * @Brief: create a new array in the server.
 * @Param: name: unique name identifying the vector.
 * @Param: n: size of the vector that is going to be created.
 * @Return: 0 upon success; -1 otherwise.
 */
int init(char *name, int n) {
    // We open the queue with the server if it didn't exist before.
    if (!server_queue)
        if ((server_queue = mq_open(srv_queue_name, O_WRONLY)) == -1)
            return -1;

    // We create the client queue if it didn't exist before.
    if (!client_queue) {
        // Since the communication process takes place inside one machine we can
        // use the PID to identify the queue. Because the PID is unique we avoid
        // naming collisions when having several clients.
        cli_queue_name = (char *) calloc(MAX_NAME_LENGTH, sizeof(char));
        sprintf(cli_queue_name, "/%d", getpid());

        // We set the arributes of the client queue.
        struct mq_attr cli_attr;
        cli_attr.mq_maxmsg = MAX_QUEUE_SIZE;
        cli_attr.mq_msgsize = sizeof(msg_t);
        if ((client_queue = mq_open(cli_queue_name, O_CREAT|O_RDONLY, 0777,
            &cli_attr)) == -1)
            return -1;
    }

    // Composition of the message.
    msg_t init_msg, init_response;
    bzero((char *) &init_msg, sizeof(msg_t)); // Clean the init_msg.

    init_msg.service = INIT;
    // We need to check if the user input has a valid lenght.
    if (strlen(name) < MAX_NAME_LENGTH)
        memcpy(init_msg.vector_name, name, strlen(name));
    else
        memcpy(init_msg.vector_name, name, MAX_NAME_LENGTH - 1);

    memcpy(init_msg.response_queue, cli_queue_name, strlen(cli_queue_name));
    init_msg.index = (uint32_t) n;
    init_msg.vector_value = 0;
    init_msg.error = 0;

    // Sending the message.
    if (mq_send(server_queue, (const char *) &init_msg, sizeof(msg_t), 0) == -1)
        return -1;
    else if (mq_receive(client_queue, (char *) &init_response, sizeof(msg_t),
            0) == -1)
        return -1;
    
    return init_response.error;
/*
    else if (init_response.error == -1) // Receiving the response.
        return -1;

    return 0;*/
}

/*
 * @Brief: set the value of a remote vector.
 * @Param: name: unique name identifying the vector.
 * @Param: i: index where the new value is going to be set.
 * @Param: value: the value that is going to be stored.
 * @Return: 0 upon success; -1 otherwise.
 */
int set(char *name, int i, int value) {
    // Composition of the message.
    msg_t set_msg, set_response;
    bzero((char *) &set_msg, sizeof(msg_t)); // Clean the set_msg.

    set_msg.service = SET;
    memcpy(set_msg.vector_name, name, strlen(name));
    memcpy(set_msg.response_queue, cli_queue_name, strlen(cli_queue_name));
    set_msg.index = (uint32_t) i;
    set_msg.vector_value = value;
    set_msg.error = 0;

    // Sending the message.
    if (mq_send(server_queue, (const char *) &set_msg, sizeof(msg_t), 0) == -1)
        return -1;
    else if (mq_receive(client_queue, (char *) &set_response, sizeof(msg_t),
        0) == -1)
        return -1;

    return set_response.error;
/*
    else if (set_response.error == -1) // Receiving the response.
        return -1;

    return 0;*/
}

/*
 * @Brief: get a value from a distributed vector.
 * @Param: name: unique name identifying the vector.
 * @Param: i: index where the value is stored.
 * @Param: value: memory position where the retreived number is going to be
 *  stored.
 * @Return: 0 upon success; -1 otherwise.
 */
int get(char *name, int i, int *value) {
    // Composition of the message.
    msg_t get_msg, get_response;
    bzero((char *) &get_msg, sizeof(msg_t)); // Clean the set_msg.

    get_msg.service = GET;
    memcpy(get_msg.vector_name, name, strlen(name));
    memcpy(get_msg.response_queue, cli_queue_name, strlen(cli_queue_name));
    get_msg.index = (uint32_t) i;
    get_msg.vector_value = 0;
    get_msg.error = 0;

    // Sending the message.
    if (mq_send(server_queue, (const char *) &get_msg, sizeof(msg_t), 0) == -1)
        return -1;
    else if (mq_receive(client_queue, (char *) &get_response, sizeof(msg_t),
        0) == -1)
        return -1;

   // else if (get_response.error == -1) // Receiving the response.
   //     return -1;

    *value = get_response.vector_value;

    return get_response.error;
    //return 0;
}

/*
 * @Brief: delete a distributed vector.
 * @Param: name: unique name identifying the vector.
 * @Return: 0 upon success; -1 otherwise.
 */
int destroy(char *name) {
    // Composition of the message.
    msg_t destroy_msg, destroy_response;
    bzero((char *) &destroy_msg, sizeof(msg_t)); // Clean the set_msg.

    destroy_msg.service = KILL;
    // We need to check if the user input has a valid lenght.
    if (strlen(name) < MAX_NAME_LENGTH)
        memcpy(destroy_msg.vector_name, name, strlen(name));
    else
        memcpy(destroy_msg.vector_name, name, MAX_NAME_LENGTH - 1);

    memcpy(destroy_msg.response_queue, cli_queue_name, strlen(cli_queue_name));
    destroy_msg.index = 0;
    destroy_msg.vector_value = 0;
    destroy_msg.error = 0;

    // Sending the message.
    if (mq_send(server_queue, (const char *) &destroy_msg, sizeof(msg_t), 0) == -1)
        return -1;
    else if (mq_receive(client_queue, (char *) &destroy_response,
        sizeof(msg_t), 0) == -1)
        return -1;

    return destroy_response.error;/*
    else if (destroy_response.error == -1) // Receiving the response.
        return -1;

    return 0;*/
}
