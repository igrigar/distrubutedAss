#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "array.h"

//#define DEBUG

int main() {

    int i;
    int j;
    char *name;
    name = (char *) calloc(16, sizeof(char));

    // Tweak theese variables to modify the behaviour of base test.
    int num_vectors = 2048;
    int size = 4194304;

    printf("+ ----------- +\n");
    printf("| Vector INIT |\n");
    printf("+ ----------- +\n");

    for (i = 0; i < num_vectors; ++i) {
        int ret;
        #ifdef DEBUG
        printf("Creating vector: \"v_%d\".\n", i);
        #endif
        sprintf(name, "v_%d", i);
        ret = init(name, size);
        if (ret == -1)
            printf("[err] %s init.\n", name);
        if (ret == 0)
            printf("[err] %s already exists.\n", name);

    }

    printf("+ ---------- +\n");
    printf("| Vector SET |\n");
    printf("+ ---------- +\n");

    for (i = 0; i < num_vectors; ++i) {
        sprintf(name, "v_%d", i);
        for (j = 0; j < size; ++j) {
            #ifdef DEBUG
            printf("set %d %d\n", j, j);
            #endif
            if (set(name, j, j) == -1)
                printf("[err] set %s[%d] = %d.\n", name, j, j); 
        }
    }

    printf("+ ---------- +\n");
    printf("| Vector GET |\n");
    printf("+ ---------- +\n");

    for (i = 0; i < num_vectors; ++i) {
        sprintf(name, "v_%d", i);
        for (j = 0; j < size; ++j) {
            int ret;
            if (get(name, j, &ret) == -1)
                printf("[err] get %s[%d] = %d.\n", name, j, j);
            else if (ret != j)
                printf("[err] get %s[%d] = %d.\n", name, j, j);
            #ifdef DEBUG
            printf("get %d %d\n", j, ret);
            #endif
        }
    }

    printf("+ ----------- +\n");
    printf("| Vector KILL |\n");
    printf("+ ----------- +\n");

    for (i = 0; i < num_vectors; ++i) {
        #ifdef DEBUG
        printf("Destroyinging vector: \"v_%d\".\n", i);
        #endif
        sprintf(name, "v_%d", i);
        if (destroy(name) == -1)
            printf("[err] destroy \"%s\".\n", name);
    }

    printf("+ ------------ +\n");
    printf("| Corner Cases |\n");
    printf("+ ------------ +\n");

    int ret;

    if (init("vector", 8) != 1)
        printf("[err] Corner case %d.\n", 1);
    if (init("vector", 8) != 0)
        printf("[err] Corner case %d.\n", 2);
    if (init("vector", 16) != -1)
        printf("[err] Corner case %d.\n", 3);
    if (init("long_vector_name", 8) != 1)
        printf("[err] Corner case %d.\n", 4);
    if (set("vector", 0, 8) != 0)
        printf("[err] Corner case %d.\n", 5);
    if (set("vector", 16, 8) != -1)
        printf("[err] Corner case %d.\n", 6);
    if (get("vector", 0, &ret) != 0)
        printf("[err] Corner case %d.\n", 7);
    if (get("vector", 7, &ret) != 0)
        printf("[err] Corner case %d.\n", 8);
    if (get("vector", 8, &ret) != -1)
        printf("[err] Corner case %d.\n", 9);
    if (destroy("vector") != 1)
        printf("[err] Corner case %d.\n", 10);
    if (destroy("vector") != -1)
        printf("[err] Corner case %d.\n", 11);
    if (destroy("long_vector_name") != 1)
        printf("[err] Corner case %d.\n", 12);

    return 0;
}
