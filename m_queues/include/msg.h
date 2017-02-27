#ifndef MSG_
#define MSG_

/******************************************************************************
 *                            Constants Definitions                           *
 ******************************************************************************/

#define MAX_QUEUE_SIZE 15
#define MAX_NAME_LENGTH 16

//const uint8_t MAX_QUEUE_SIZE = 15; // Define the maximum lenght of a queue.
//const uint8_t MAX_NAME_LENGTH = 16; // Maximum lenght of a vector name.

// Name of the queue for communicating with the server.
const char srv_queue_name[MAX_NAME_LENGTH] = "/dist_vec_queue";


/******************************************************************************
 *                            Structure Definitions                           *
 ******************************************************************************/

// Services provided by the server.
typedef enum {INIT, SET, GET, KILL} msg_code;

// Struct that defines a message of the queue.
typedef struct {
    msg_code service; // Operation requested to the server.
    char vector_name[MAX_NAME_LENGTH]; // Name of vector we want to operate on.
    char response_queue[MAX_NAME_LENGTH]; // Name of queue used to response.
    uint32_t index; // Index of the distributed vector.
    int vector_value; // Value of the vector at 'index'.
                      // Used also to provide the size of the vector in INIT.
    int8_t error; // Check if the operations produce errors.
} msg_t;

#endif
