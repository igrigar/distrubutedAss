#include <string.h>
#include <stdlib.h>
#include <stdio.h> // Eliminar al final.

#include "list.h"

/*
 * @Brief: add a user to de system. This means inserting an entry in the list.
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
    current->seq = 0;
    current->port = 0;
    current->ip.s_addr = 0;

    current->next->next = NULL;
    current->next->prev = current;

    // Handle message list allocation.

    return 0;
}

/*
 * @Brief: delete a user from the system. This means deleting it's entry in the
 *  list.
 *
 * @Return 0: if everything goes well.
 * @Return 1: if the user does not exist.
 */
uint8_t rm_usr(node_t **head, char *name) {
    node_t *current = *head;

    while (strcmp(current->usr, name) != 0) // Looking for the name.
        if (current->next == NULL) return 1; // User not found.
        else current = current->next;

    if (current->prev == NULL) { // Head node.
        current->next->prev = NULL;
        *head = current->next;
    }
    else current->prev->next = current->next;

    free(current);
    return 0;
}

/*
 * @Brief: connect a user to the system. This means modify it's status and
 *  setting up it's IP address and port.
 *
 * @Return 0: if everything goes well.
 * @Return 1: if the user does not exist.
 * @Return 2: if the user was already online.
 */
uint8_t conn_usr(node_t *head, char *name, struct in_addr ip, uint8_t port) {
    node_t *current = head;

    while (strcmp(current->usr, name) != 0) // Looking for the name.
        if (current->next == NULL) return 1; // User not found.
        else current = current->next;

    if (current->status == ONLINE)
        return 1;

    current->status = ONLINE;
    current->port = htons(port);
    current->ip = ip;

    return 0;
}

/*
 * @Brief: disconnect a user from the system. This means to modify it's
 *  status and "wiping" IP address and port.
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
