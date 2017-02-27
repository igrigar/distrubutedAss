#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "array.h"

int main() {

    printf("Client test mode.\n");

    printf("Init Test.\n");

    printf("Creating vectors.\n");
    init("vector_1", 8); // Should work.
    init("vector_2", 8); // Should work.
    init("vector_1", 8); // Should not work.
    init("vector_3", 8); // Should work.
    init("vector_4", 8); // Should work.
    init("vector_5", 8); // Should not work.

    printf("Destroying vectors.\n");
    destroy("vector_1"); // Should work.
    destroy("vector_5"); // Should not work.
    destroy("vector_2"); // Should work.
    destroy("vector_3"); // Should work.
    destroy("vector_1"); // Should not work.
    destroy("vector_4"); // Should work.

    int size = 16; // Size of a new vector.
    int i; // Index of the loop.
    int value;

    init("vector", size);
    for(i = 0; i < 32; ++i) {
        set("vector", i, i);
        get("vector", i, &value);
        if (i != value) printf("ERROR GET\n");
    }
    destroy("vector");

    return 0;
}
