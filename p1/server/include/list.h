#ifndef _USER_LIST_
#define _USER_LIST_

#include <stdint.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFFER_SIZE 256 // Maximum buffer size.

typedef enum status {ONLINE, OFFLINE} status_t;

// Node of message list.
typedef struct  msg {
    char message[BUFFER_SIZE]; // Message itself.
    char from[BUFFER_SIZE]; // User sending the message.
    char md5[BUFFER_SIZE]; // Checksum of the message.
    uint32_t seq;

    struct msg *next;
} msg_t;

// Node of user list.
typedef struct node {
    char usr[BUFFER_SIZE]; // Name of the user.
    status_t status; // Status in the system
    uint8_t seq; // MSG sequence number.    int port; // Port to send messages.
    int port; // Port to send messages.
    struct in_addr ip; // IP to send messages.
    msg_t *msg_list; // List of messages pendant.

    // Concurrency stuff.
    pthread_mutex_t lock;
    pthread_cond_t unlock;
    uint8_t busy; // Variable for the condition.

    struct node *next; // Next node in the list.
    struct node *prev; // Previous node in the list.
} node_t;

// User list related functions.
uint8_t add_usr(node_t *head, char  *name);
uint8_t rm_usr(node_t **head, char *name);
node_t * get_user(node_t *head, char *name);

// Message list related functions.
void add_msg(node_t *head, char *sender, char *receiver, char *message, char *md5, uint32_t seq);
void append_msg (msg_t *head, char *sender, char *message,char *md5, uint32_t seq);
msg_t * pop_msg(node_t *head, char *name);

void debug(node_t *head);
#endif
