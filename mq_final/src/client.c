#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "array.h"

#define DEBUG

int main() {

    int i, j;
    int num_vectors = 1024;
    int size = 4096;
    char *name;
    name = (char *) calloc(16, sizeof(char));

    printf("+---------------------------+\n");
    printf("|        Vector INIT        |\n");
    printf("+---------------------------+\n");

     for (i = 0; i < num_vectors; ++i) {
        #ifdef DEBUG
        printf("Creating vector: \"v_%d\".\n", i);
        #endif
        sprintf(name, "v_%d", i);
        init(name, size);
    }

    printf("+---------------------------+\n");
    printf("|        Vector SET         |\n");
    printf("+---------------------------+\n");

    for (i = 0; i < num_vectors; ++i) {
        sprintf(name, "v_%d", i);
        for (j = 0; j < size; ++j) {
            #ifdef DEBUG
            printf("Setting: v_%d[%d] = %d.\n", i, j, j);
            #endif
            set(name, j, j);
        }
    }

    printf("+---------------------------+\n");
    printf("|        Vector GET         |\n");
    printf("+---------------------------+\n");

    for (i = 0; i < num_vectors; ++i) {
        sprintf(name, "v_%d", i);
        for (j = 0; j < size; ++j) {
            int ret;
            get(name, j, &ret);
            #ifdef DEBUG
            printf("Setting: v_%d[%d] = %d.\n", i, j, ret);
            #endif
        }
    }

    printf("+---------------------------+\n");
    printf("|        Vector KILL        |\n");
    printf("+---------------------------+\n");

    for (i = 0; i < num_vectors; ++i) {
        #ifdef DEBUG
        printf("Creating vector: \"v_%d\".\n", i);
        #endif
        sprintf(name, "v_%d", i);
        destroy(name);
    }

    return 0;
}
