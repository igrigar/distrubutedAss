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

/******************************************************************************
 *                              FUNCTION HEADERS                              *
 ******************************************************************************/

void session_handler(void *args);
uint8_t check_port(char *port);
ssize_t read_line(int fd, void *buffer, int n);
ssize_t write_line(int fd, void *buffer, size_t n);

void debug();

/******************************************************************************
 *                          FUNCTION IMPLEMENTATION                           *
 ******************************************************************************/

int main(int argc, char **argv) {

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
    char ret = '3'; // Error by default.

    printf("Handling connection: %d.\n", socket);

    // Regular operations.
    read_line(socket, buffer, BUFFER_SIZE); // Getting the Operation Request.

    // We handle the request.
    if (!strcmp(buffer, "REGISTER")) {
        read_line(socket, buffer, BUFFER_SIZE); // Read the username.
        if (!usr_list) { // List is empty.
            printf("Empty List.\n");
            usr_list = (node_t *) malloc(sizeof(node_t));
            memcpy(usr_list->usr, buffer, strlen(buffer));
            usr_list->status = OFFLINE;
            usr_list->seq = 0;
            usr_list->port = 0;
            usr_list->ip.s_addr = 0;

            usr_list->next = NULL;
            usr_list->prev = NULL;
            ret = '0';
        }
        else sprintf(&ret, "%d", add_usr(usr_list, buffer));
    } else if (!strcmp(buffer, "UNREGISTER")) {
        read_line(socket, buffer, BUFFER_SIZE); // Read the username.
        if (!usr_list) ret = '2'; // List not initialized, so user not in sys.
        else sprintf(&ret, "%d", rm_usr(&usr_list, buffer));
    } else if (!strcmp(buffer, "CONNECT")) {
        char port[8];
        uint8_t port_num;
        read_line(socket, buffer, BUFFER_SIZE); // Read the username.
        read_line(socket, port, 8); // Read the port.
        printf("Registring %s on port %s\n", buffer, port);
        sscanf(port, "%c", &port_num);
        if (!usr_list) ret = '2'; // List not initialized, so user not in sys.
        else sprintf(&ret, "%d", conn_usr(usr_list, buffer, arg->cli_addr,
            port_num));
    } else if (!strcmp(buffer, "DISCONNECT")) {
        read_line(socket, buffer, BUFFER_SIZE); // Read the username.
        if (!usr_list) ret = '2'; // List not initialized, so user not in sys.
        else sprintf(&ret, "%d", disconn_usr(usr_list, buffer));
    } else {
        printf("COMMAND UNKNOWN.\n");
    }

    debug();
    write_line(socket, &ret, 1);
    close(socket);
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

    if (!buffer) {
        errno = EINVAL;
        return -1;
    }

    buff = buffer;

    while (length > 0 && last_write >= 0) {
        last_write = write(fd, buff, length);
        length -= last_write;
        buff += last_write;
    }

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


/** ELIMINAR ESTA MIERDA ***/
void debug() {
    node_t *current = usr_list;
    printf("List of users:\n");
    while (current != NULL) {
        switch (current->status) {
            case ONLINE:
                printf("%s: connected.\n", current->usr); break;
            case OFFLINE:
                printf("%s: disconnected.\n", current->usr); break;
        }
        current = current->next;
    }
    printf("\n");
}
