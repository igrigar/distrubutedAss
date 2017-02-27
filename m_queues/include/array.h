#ifndef ARRAY_
#define ARRAY_

#include <stdint.h>

//#define MAX_QUEUE_SIZE 10
//#define MAX_NAME_LENGTH 16

/******************************************************************************
 *                             Function Prototypes                            *
 ******************************************************************************/

int init(char *name, int n);
int set(char *name, int i, int value);
int get(char *name, int i, int *value);
int destroy(char *name);

#endif
