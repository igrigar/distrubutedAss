#ifndef _USER_LIST_
#define _USER_LIST_

#include <stdint.h>
#include <netinet/in.h>

typedef enum {ONLINE, OFFLINE} status_t;

typedef struct node {
    char usr[64]; // Name of the user.
    status_t status; // Status in the system
    uint8_t seq; // MSG sequence number.
    uint32_t port; // Port to send messages.
    struct in_addr ip; // IP to send messages.
    // LIST OF MESSAGES.

    struct node *next; // Next node in the list.
    struct node *prev; // Previous node in the list.
} node_t;

uint8_t add_usr(node_t *head, char  *name);
uint8_t rm_usr(node_t **head, char *name);
uint8_t conn_usr(node_t *head, char *name, struct in_addr ip, uint8_t port);
uint8_t disconn_usr(node_t *head, char *name);
#endif
