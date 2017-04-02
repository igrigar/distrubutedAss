#ifndef _USER_LIST_
#define _USER_LIST_

#include <stdint.h>
#include <netinet/in.h>

typedef enum status {ONLINE, OFFLINE} status_t;

// Node of message list.
typedef struct  msg {
    char message[256]; // Message itself.
    char from[256]; // User sending the message.
    uint32_t seq;

    struct msg *next;
} msg_t;

// Node of user list.
typedef struct node {
    char usr[64]; // Name of the user.
    status_t status; // Status in the system
    uint8_t seq; // MSG sequence number.
    int port; // Port to send messages.
    struct in_addr ip; // IP to send messages.
    msg_t *msg_list; // List of messages pendant.

    struct node *next; // Next node in the list.
    struct node *prev; // Previous node in the list.
} node_t;

// User list related functions.
uint8_t add_usr(node_t *head, char  *name);
uint8_t rm_usr(node_t **head, char *name);
uint8_t conn_usr(node_t *head, char *name, struct in_addr ip, int port);
uint8_t disconn_usr(node_t *head, char *name);

// Helper user list functions.
uint8_t usr_exists(node_t *head, char *name);
uint32_t get_seq_num(node_t *head, char *name);
void update_seq_num(node_t *head, char *name);
node_t * get_user(node_t *head, char *name);

// Message list related functions.
void add_msg(node_t *head, char *sender, char *receiver, char *message, uint32_t seq);
void append_msg (msg_t *head, char *sender, char *message, uint32_t seq);
msg_t * pop_msg(node_t *head, char *name);

#endif
