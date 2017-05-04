#include <string.h>
#include <stdlib.h>

#include "list.h"

/*
 * @Brief: add a user to de system. This means inserting an entry in the list.
 *
 * @Param head: head node of the list.
 * @Param name: name of the user we are looking for.
 *
 * @Return 0: if everything goes well.
 * @Return 1: if the user already exist.
 * @Return 2: error creating the node.
 */
uint8_t add_usr(node_t *head, char *name) {
    node_t *current = head;

    while (current->next != NULL) // Traverse the list.
        if (!strcmp(current->usr, name)) return 1; // User already registered.
        else current = current->next;

    if (!strcmp(current->usr, name)) return 1; // User already registered.

    // User was not in the list, so we create it.
    current->next = (node_t *) malloc(sizeof(node_t)); // Allocate memory.

    strcpy(current->next->usr, name);
    current->next->status = OFFLINE;
    current->next->seq = 1;
    current->next->port = 0;
    current->next->ip.s_addr = 0;
    current->next->msg_list = NULL;
    current->next->next = NULL;
    current->next->prev = current;

    current->next->busy = 0;
    if (pthread_mutex_init(&current->next->lock, NULL) != 0 ||
            pthread_cond_init(&current->next->unlock, NULL) != 0) {
        free(current->next);
        return 2;
    }

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

#include <stdio.h>
void debug(node_t *head) {
    node_t *current = head;

    while (current) {
        printf("Name: %s\n", current->usr);
        if (current->status == ONLINE) printf("Status: Online\n");
        else printf("Status: Offline\n");
        printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");

        current = current->next;
    }
}

/*
 * @Brief: add a message to the list of messages.
 *
 * @Param *head: pointer to the head of the list of users.
 * @Param *message: message to be stored.
 * @Param *sender: name of the user who sent the message.
 * @Param *receiver: name of the user who receives the message.
 * @Param seq: sequence number associated to the message.
 */
void add_msg(node_t *head, char *sender, char *receiver, char *message, uint32_t seq) {
    node_t *current = head;

    while (strcmp(current->usr, receiver) != 0) // Looking for the name.
        if (current->next == NULL) return; // User not found.
        else current = current->next;

    if (current->msg_list == NULL) { // No head in the message list.
        current->msg_list = (msg_t *) malloc(sizeof(msg_t));

        // Clean string fields.
        bzero(current->msg_list->message, 256);
        bzero(current->msg_list->from, 256);

        //memcpy(current->msg_list->message, message, strlen(message));
        //memcpy(current->msg_list->from, sender, strlen(sender));
        strcpy(current->msg_list->message, message);
        strcpy(current->msg_list->from, sender);

        current->msg_list->seq = seq;
        current->msg_list->next = NULL;
    } else append_msg(current->msg_list, sender, message, seq);
}

/*
 * @Brief: add a message to the list of messages.
 *
 * @Param *head: pointer to the head of the list of users.
 * @Param *message: message to be stored.
 * @Param *sender: name of the user who sent the message.
 * @Param *receiver: name of the user who receives the message.
 * @Param seq: sequence number associated to the message.
 */
void append_msg (msg_t *head, char *sender, char *message, uint32_t seq) {
    msg_t *current = head;

    if (head == NULL) { // Head node.
        head = (msg_t *) malloc(sizeof(msg_t));

        // Wipe fields.
        bzero(head->message, BUFFER_SIZE);
        bzero(head->from, BUFFER_SIZE);

        strcpy(head->message, message);
        strcpy(head->from, sender);

        head->seq = seq;
        head->next = NULL;

        return;
    }

    while (current->next != NULL) current = current->next; // Traverse the list.

    current->next = (msg_t *) malloc(sizeof(msg_t));

    // Clean fields
    bzero(current->next->message, BUFFER_SIZE);
    bzero(current->next->from, BUFFER_SIZE);

    strcpy(current->next->message, message);
    strcpy(current->next->from, sender);

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
