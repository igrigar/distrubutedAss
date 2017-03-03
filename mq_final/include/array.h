#ifndef ARRAY_
#define ARRAY_

int init(char *name, int n);
int set(char *name, int i, int value);
int get(char *name, int i, int *value);
int destroy(char *name);

#endif
