#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "array.h"

int main(int argc, char **argv) {

    printf("Client test mode.\n");

    int size = 16; // Size of a new vector.
    int i; // Index of the loop.
    int value;

    init("vector", size);
    for(i = 0; i < 32; ++i) {
        set("vector", i, i);
        get("vector", i, &value);
        if (i != value) printf("ERROR GET\n");
        sleep(1);
    }
    destroy("vector");

    return 0;
}
