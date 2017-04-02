#include <string.h>
#include <stdlib.h>

#include <stdio.h>

#include "list.h"

/*
 * @Brief: add a user to de system. This means inserting an entry in the list.
 *
 * @Param head: head node of the list.
 * @Param name: name of the user we are looking for.
 *
 * @Return 0: if everything goes well.
 * @Return 1: if the user does not exist.
 */
uint8_t add_usr(node_t *head, char *name) {
    node_t *current = head;

    while (current->next != NULL) // Traverse the list.
        if (!strcmp(current->usr, name)) return 1; // User already registered.
        else current = current->next;

    if (!strcmp(current->usr, name)) return 1; // User already registered.

    // User was not in the list, so we create it.
    current->next = (node_t *) malloc(sizeof(node_t)); // Allocate memory.
    memcpy(current->next->usr, name, strlen(name));
    current->next->status = OFFLINE;
    current->next->seq = 1;
    current->next->port = 0;
    current->next->ip.s_addr = 0;
    current->next->msg_list = NULL;
    current->next->next = NULL;
    current->next->prev = current;

    return 0;
}

/*
 * @Brief: delete a user from the system. This means deleting it's entry in the
 *  list.
 *
 * @Param head: pointer head node of the list.
 * @Param name: name of the user we are looking for.
 *
 * @Return 0: if everything goes well.
 * @Return 1: if the user does not exist.
 * @Return 2: if the user is connected.
 */
uint8_t rm_usr(node_t **head, char *name) {
    node_t *current = *head;

    while (strcmp(current->usr, name) != 0) // Looking for the name.
        if (current->next == NULL) return 1; // User not found.
        else current = current->next;

    if (current->status == ONLINE) return 2;

    if (current->prev == NULL) { // Head node.
        if (current->next) {
            current->next->prev = NULL;
            *head = current->next;
        } else { // Last element of the list
            *head = NULL;
        }
    }
    else current->prev->next = current->next;

    free(current);
    return 0;
}

/*
 * @Brief: connect a user to the system. This means modify it's status and
 *  setting up it's IP address and port.
 *
 * @Param head: head node of the list.
 * @Param name: name of the user we are looking for.
 * @Param in_addr: IP addres of client's message listener.
 * @Param port: port of client's message listener.
 *
 * @Return 0: if everything goes well.
 * @Return 1: if the user does not exist.
 * @Return 2: if the user was already online.
 */
uint8_t conn_usr(node_t *head, char *name, struct in_addr ip, int port) {
    node_t *current = head;

    while (strcmp(current->usr, name) != 0) // Looking for the name.
        if (current->next == NULL) return 1; // User not found.
        else current = current->next;

    if (current->status == ONLINE)
        return 1;

    current->status = ONLINE;
    current->port = port;
    current->ip = ip;

    return 0;
}

/*
 * @Brief: disconnect a user from the system. This means to modify it's
 *  status and "wiping" IP address and port.
 *
 * @Param head: head node of the list.
 * @Param name: name of the user we are looking for.
 *
 * @Return 0: if everything goes well.
 * @Return 1: if the user does not exist.
 * @Return 2: if the user was already offline.
 */
uint8_t disconn_usr(node_t *head, char *name) {
    node_t *current = head;

    while (strcmp(current->usr, name) != 0) // Looking for the name.
        if (current->next == NULL) return 1; // User not found.
        else current = current->next;

    if (current->status == OFFLINE)
        return 2;

    current->status = OFFLINE;
    current->port = 0;
    current->ip.s_addr = 0;

    return 0;
}

/*
 * @Brief: looks for a user inside the list, it also checks if it is connected.
 *
 * @Param head: head node of the list.
 * @Param name: name of the user we are looking for.
 *
 * @Return 0: when user is found and it is connected.
 * @Return 1: when user is found and is not connected.
 * @Return 2: when not found.
 */
uint8_t usr_exists(node_t *head, char *name) {
    node_t *current = head;

    while (strcmp(current->usr, name) != 0) // Looking for the name.
        if (current->next == NULL) return '2'; // User not found.
        else current = current->next;

    if (current->status == ONLINE) return '0'; // User online.
    else return '1'; // User offline.
}

/*
 * @Brief: return the sequence number that identifies a message sent by an user.
 *
 * @Param head: head node of the list.
 * @Param name: name of the user we are looking for.
 *
 * @Return 0: user does not exist. (Because this value is imposible as seq num)
 * @Return x!=0: sequence number.
 */
uint32_t get_seq_num(node_t *head, char *name) {
    node_t *current = head;

    while (strcmp(current->usr, name) != 0) // Looking for the name.
        if (current->next == NULL) return 0; // User not found.
        else current = current->next;

    return current->seq;
}

/*
 * @Brief: increase the sequence number of messages for a user.
 *
 * @Param head: head node of the list.
 * @Param name: name of the user we are looking for.
 */
void update_seq_num(node_t *head, char *name) {
    node_t *current = head;

    while (strcmp(current->usr, name) != 0) // Looking for the name.
        if (current->next == NULL) return; // User not found.
        else current = current->next;

    if (++current->seq == 0) current->seq = 1; // Overflow, so set to 1.
}

/*
 * @Brief: obtain a user from the list.
 *
 * @Param *head: pointer to the head of the list of users.
 * @Param *name: name of the owner of the message to be sent.
 *
 * @Return: pointer to the user struct (NULL if list empty).
 */
node_t * get_user(node_t *head, char *name) {
    node_t *current = head;

    while (strcmp(current->usr, name) != 0) // Looking for the name.
        if (current->next == NULL) return NULL; // User not found.
        else current = current->next;

    return current;
}

/*
 * @Brief: add a message to the list of messages.
 *
 * @Param *head: pointer to the head of the list of users.
 * @Param *msg: message to be stored.
 * @Param *name: name of the user who sent the message
 */
void add_msg(node_t *head, char *sender, char *receiver, char *message, uint32_t seq) {
    node_t *current = head;

    while (strcmp(current->usr, receiver) != 0) // Looking for the name.
        if (current->next == NULL) return; // User not found.
        else current = current->next;

    if (current->msg_list == NULL) { // No head in the message list.
        current->msg_list = (msg_t *) malloc(sizeof(msg_t));
        memcpy(current->msg_list->message, message, strlen(message));
        memcpy(current->msg_list->from, sender, strlen(sender));
        current->msg_list->seq = seq;
        current->msg_list->next = NULL;
    } else append_msg(current->msg_list, sender, message, seq);
    /*{ // Append to the message list.
        printf("1\n");
        msg_t *msg = current->msg_list;
        printf("2\n");
        while (msg->next != NULL)
            printf(".\n");
            msg = msg->next;
        printf("3\n");

        msg->next = (msg_t *) malloc(sizeof(msg_t));
        printf("4\n");
        memcpy(msg->next->message, message, strlen(message));
        printf("5\n");
        memcpy(msg->next->from, name, strlen(name));
        printf("6\n");
        msg->seq = seq;
        printf("7\n");
        msg->next->next = NULL;
    }*/
}

void append_msg (msg_t *head, char *sender, char *message, uint32_t seq) {
    msg_t *current = head;

    while (current->next != NULL) current = current->next; // Traverse the list.

    current->next = (msg_t *) malloc(sizeof(msg_t));
    memcpy(current->next->message, message, strlen(message));
    memcpy(current->next->from, sender, strlen(sender));
    current->next->seq = seq;
    current->next->next = NULL;
}

/*
 * @Brief: obtain a message from the message list of a user.
 *
 * @Param *head: pointer to the head of the list of users.
 * @Param *name: name of the owner of the message to be sent.
 *
 * @Return: pointer to the message struct (NULL if list empty).
 */
msg_t * pop_msg(node_t *head, char *name) {
    node_t *current = head;

    while (strcmp(current->usr, name) != 0) // Looking for the name.
        if (current->next == NULL) return NULL; // User not found.
        else current = current->next;

    if (current->msg_list == NULL) return NULL; // List is empty.

    msg_t *ret = current->msg_list;
    msg_t *next_msg = current->msg_list->next;

    free(current->msg_list);
    current->msg_list = next_msg;

    return ret;
}
