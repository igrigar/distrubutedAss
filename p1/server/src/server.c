#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>

#include "list.h"

/******************************************************************************
 *                              GLOBAL VARIABLES                              *
 ******************************************************************************/

// Information needed to handle the requests.
typedef struct {
    int fd;
    struct in_addr cli_addr;
} arg_t;

node_t *usr_list = NULL; // List of users.

const uint16_t BUFFER_SIZE = 256; // Maximum buffer size.
int _port;
int _socket;
int _cli_size;

struct sockaddr_in srv_addr;


pthread_mutex_t user_list_lck;
pthread_cond_t user_list_ulck;
uint32_t user_list_worker = 0;


/******************************************************************************
 *                              FUNCTION HEADERS                              *
 ******************************************************************************/

void session_handler(void *args);
void user_register(int socket);
void user_unregister(int socket);
void user_connect(int socket, struct in_addr cli_addr);
void user_disconnect(int socket);
void rcv_message(int socket);
void flush_msg_list(char *name);
uint8_t check_port(char *port);
ssize_t read_line(int fd, void *buffer, int n);
ssize_t write_line(int fd, void *buffer, size_t n);
void u_list_lck();
void u_list_ulck();


/******************************************************************************
 *                          FUNCTION IMPLEMENTATION                           *
 ******************************************************************************/

int main(int argc, char **argv) {

    // Check args.
    if (argc != 3 || strcmp(argv[1], "-p") || !check_port(argv[2])) {
        printf("Usage: ./server -p <port>\n");
        exit(-1);
    }


    _port = atoi(argv[2]);
    _socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4
                                               // SOCK_STREAM = TCP
    if (_socket < 0) { // Comprobar el descriptor de fichero.
        printf("[ERR] Socket Open.\n");
        exit(-1);
    }

    // Create the socket.
    bzero((char *) &srv_addr, sizeof(srv_addr)); // Clean the structure.
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_port = htons(_port);
    bind(_socket, (const struct sockaddr *) &srv_addr, sizeof(srv_addr));

    listen(_socket, SOMAXCONN); // Start listening to new connections.
    _cli_size = sizeof(struct sockaddr_in);


    // Initialize concurrency variables.
    if (pthread_mutex_init(&user_list_lck, NULL) != 0) {
        perror("User list mutex");
        exit(-1);
    }
    if (pthread_cond_init(&user_list_ulck, NULL) != 0) {
        perror("User list cond");
        exit(-1);
    }

    while (1) {
        int client_socket;
        struct sockaddr_in cli_addr;
        arg_t args;

        // New client connection.
        client_socket = accept(_socket, (struct sockaddr *) &cli_addr,
            (socklen_t *) &_cli_size);

        // Setting arguments struct.
        struct sockaddr_in *s = (struct sockaddr_in *)&cli_addr;
        args.fd = client_socket;
        args.cli_addr = s->sin_addr;

        // Creating the thread that is going to handle the session.
        pthread_t thread;

        if(pthread_create(&thread, NULL, (void *) session_handler,
                &args) == -1) {
            printf("Error creating the thread.\n");
            close(client_socket);
        }
        pthread_detach(thread);
    }

    return 0;
}

/*
 * @Brief: handle a session in the server.
 *
 * @param *fd: file descriptor of the writeback socket.
 */
void session_handler(void *args) {
    arg_t *arg = (arg_t *) args;
    int socket = arg->fd;
    char buffer[BUFFER_SIZE];

    // Regular operations.
    read_line(socket, buffer, BUFFER_SIZE); // Getting the Operation Request.

    // We handle the request.
    if (!strcmp(buffer, "REGISTER")) user_register(socket);
    else if (!strcmp(buffer, "UNREGISTER")) user_unregister(socket);
    else if (!strcmp(buffer, "CONNECT")) user_connect(socket, arg->cli_addr);
    else if (!strcmp(buffer, "DISCONNECT")) user_disconnect(socket);
    else if (!strcmp(buffer, "SEND")) rcv_message(socket);
    else printf("COMMAND UNKNOWN.\n");

    close(socket);
}

/*
 * @Brief: add a user to the list of registered users.
 *
 * @Param socket: file descriptor of the socket where the connection is created.
 */
void user_register(int socket) {
    char buffer[BUFFER_SIZE];
    uint8_t response;

    read_line(socket, buffer, BUFFER_SIZE); // Read the username.

    u_list_lck();
    if (!usr_list) { // List is empty.
        usr_list = (node_t *) malloc(sizeof(node_t));
        memcpy(usr_list->usr, buffer, strlen(buffer));
        usr_list->status = OFFLINE;
        usr_list->seq = 1;
        usr_list->port = 0;
        usr_list->ip.s_addr = 0;
        usr_list->msg_list = NULL;

        usr_list->next = NULL;
        usr_list->prev = NULL;

        response = 0;
    } else response = add_usr(usr_list, buffer);
    u_list_ulck();

    // Print returns.
    if (response == 0) printf("REGISTER %s OK.\n", buffer);
    else printf("REGISTER %s FAIL.\n", buffer);

    write_line(socket, &response, 1);
    close(socket);
}

/*
 * @Brief: set the status of a user to connected. It also sets the IP and port
 *  of client's listener.
 *
 * @Param socket: file descriptor of the socket where the connection is created.
 * @Param cli_addr: IP address of client's listener.
 */
void user_connect(int socket, struct in_addr cli_addr) {
    char buffer[BUFFER_SIZE], port[8];
    uint8_t response;

    read_line(socket, buffer, BUFFER_SIZE); // Read the username.
    read_line(socket, port, 8); // Read the port.

    u_list_lck();
    if (!usr_list) response = 2; // List not initialized, so user not in sys.
    else response = conn_usr(usr_list, buffer, cli_addr, atoi(port));
    u_list_ulck();

    // Print returns.
    if (response == 0) {
        printf("CONNECT %s OK.\n", buffer);
        flush_msg_list(buffer); // Send the pendant messages.
    } else printf("CONNECT %s FAIL.\n", buffer);

    write_line(socket, &response, 1);
    close(socket);
}

/*
 * @Brief: set the status of a user to disconnected.
 *
 * @Param socket: file descriptor of the socket where the connection is created.
 */
void user_disconnect(int socket) {
    char buffer[BUFFER_SIZE];
    uint8_t response;

    read_line(socket, buffer, BUFFER_SIZE); // Read the username.

    u_list_lck();
    if (!usr_list) response = 2; // List not initialized, so user not in sys.
    else response = disconn_usr(usr_list, buffer);
    u_list_ulck();

    // Print returns.
    if (response == 0) printf("DISCONNECT %s OK.\n", buffer);
    else printf("DISCONNECT %s FAIL.\n", buffer);

    write_line(socket, &response, 1);
    close(socket);
}

/*
 * @Brief: remove a user from the list of registered users.
 *
 * @Param socket: file descriptor of the socket where the connection is created.
 */
void user_unregister(int socket) {
    char buffer[BUFFER_SIZE];
    uint8_t response;

    read_line(socket, buffer, BUFFER_SIZE); // Read the username.

    u_list_lck();
    if (!usr_list) response = 2; // List not initialized, so user not in sys.
    else response = rm_usr(&usr_list, buffer);
    u_list_ulck();

    // Print returns.
    if (response == 0) printf("UNREGISTER %s OK.\n", buffer);
    else printf("UNREGISTER %s FAIL.\n", buffer);

    write_line(socket, &response, 1);
    close(socket);
}

/*
 * @Brief: handle the first part of message forwarding.
 *
 * @Param socket: file descriptor of the socket where the connection is created.
 */
void rcv_message(int socket) {
    char sender[BUFFER_SIZE], receiver[BUFFER_SIZE], msg[BUFFER_SIZE];
    uint8_t response;

    // Receive all the data.
    read_line(socket, sender, BUFFER_SIZE); // Read the username.
    read_line(socket, receiver, BUFFER_SIZE); // Read the username.
    read_line(socket, msg, BUFFER_SIZE); // Read the message.

    // Check Sender.
    u_list_lck();
    if (!usr_list) response =  2; // List not initialized, so user not in sys.
    else response = usr_exists(usr_list, sender);
    u_list_ulck();

    if (response != 0) { // Sender not in system or not connected.
        printf("ERROR Sender: %d\n", response);
        write_line(socket, &response, 1);
        return;
    }

    // Check receiver.
    u_list_lck();
    if (!usr_list) response =  2; // List not initialized, so user not in sys.
    else response = usr_exists(usr_list, receiver);
    u_list_ulck();

    if (response == 2) { // Receiver not in the system.
        printf("Error Receiver\n");
        write_line(socket, &response, 1);
        return;
    } else { // Enqueue the message to receiver msg_list.
        char seq[BUFFER_SIZE], ret = 0;
        uint32_t sequence = get_seq_num(usr_list, sender);

        if (sequence == 0) {
            return; // Something went wrong.
        }
        update_seq_num(usr_list, sender);

        add_msg(usr_list, sender, receiver, msg, sequence);
        write_line(socket, &ret, 1);

        sprintf(seq, "%d", sequence);
        write_line(socket, seq, strlen(seq));
    }

    if (response == 0) flush_msg_list(receiver); // If user connected send msg.

    close(socket);
}

void flush_msg_list(char *name) {
    node_t *user = get_user(usr_list, name);
    msg_t *msg = pop_msg(usr_list, name);
    char *init = "SEND_MESSAGE";

    while (msg != NULL) {
        int sock;
        struct sockaddr_in serv_addr;
        char sequence[10];

        sock = socket(AF_INET, SOCK_STREAM, 0);
        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        memcpy((char *) &user->ip, (char *) &serv_addr.sin_addr, sizeof(struct in_addr));
        serv_addr.sin_port = htons(user->port);

        if (connect(sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
            printf("Client unreachable\n");
            flush_msg_list(name); // try again.
        }
        sprintf(sequence, "0%d", msg->seq);

        write_line(sock, init, strlen(init));
        write_line(sock, msg->from, strlen(msg->from));
        write_line(sock, sequence, strlen(sequence));
        write_line(sock, msg->message, strlen(msg->message));
        close(sock);

        msg = pop_msg(usr_list, name);
    }
}

/*
 * @Brief: write a buffer in a line.
 *
 * @Param fd: file descriptor where we are going to write.
 * @Param *buffer: buffer to store the data we want to write.
 * @Param n: size of the buffer.
 *
 * @Return: 0 on succes, -1 upon error.
 */
ssize_t write_line(int fd, void *buffer, size_t n) {
    int last_write = 0;
    size_t length = n;
    char *buff;
    char nl = '\n';

    if (!buffer) {
        errno = EINVAL;
        return -1;
    }

    buff = (char *) buffer;

    while (length > 0 && last_write >= 0) {
        last_write = write(fd, buff, length);
        length -= last_write;
        buff += last_write;
    }

   if (n > 1) write(fd, &nl, 1);

    if (last_write < 0) return -1; // Error.
    else return 0; // Write done.
}

/*
 * @Brief: read a line from a file. In this case we consider the delimiters of a
 *  line the characters: '\n' and '\0'.
 *
 * @Param fd: file descriptor from which we are going to read.
 * @Param *buffer: buffer to store the read data.
 * @Param n: size of the buffer.
 *
 * @Return: number of bytes read. -1 upon error.
 */
ssize_t read_line(int fd, void *buffer, int n) {
    int last_read;
    int total_read;
    char *buff;
    char c;

    if (n < 0 || !buffer) {
        errno = EINVAL;
        return -1;
    }

    buff = buffer;
    bzero(buff, strlen(buff));

    // We try to fill the buffer until (n - 1) to add in the end '\0'.
    for (total_read = 0; total_read < (n - 1); ++total_read) {
        last_read = read(fd, &c, 1); // Read 1 byte.

        if (last_read == -1) { // Checking read errors.
            if (errno == EINTR) continue; // interrupted -> restart read().
            else return -1;
        } else if (last_read == 0) { // We have reached EOF.
            break;
        } else {
            if (c == '\n' || c == '\0') break;
            *buff++ = c;
        }
    }

    *buff = '\0'; // Set the delimiter.
    return total_read;
}

/*
 * @Brief: check that the introduced port is indeed a positive integer number
 *  in the range (2^16 - 1) because ports are 16 bits but port 0 is reserved.
 *
 * @Return: 0 if it is not a valid port, 1 otherwise.
 */
uint8_t check_port(char *port) {
    while (*port)
        if (!isdigit(*port++)) return 0;
    // Make the < 2^16 thing here.
    return 1;
}

/*
 * @Brief: procedure to lock the list of users.
 */
void u_list_lck() {
    pthread_mutex_lock(&user_list_lck);
    while (user_list_worker)
        pthread_cond_wait(&user_list_ulck, &user_list_lck);
    ++user_list_worker;
}

/*
 * @Brief: procedure to lock the list of users.
 */
void u_list_ulck() {
    --user_list_worker;
    pthread_cond_broadcast(&user_list_ulck);
    pthread_mutex_unlock(&user_list_lck);
}
