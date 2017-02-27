# compiler flags using during compilation of source files.
# C Language options:
#     -std=c99: use the standard c99.
# Warning options:
#     -Werror: make all warnings into errors.
#     -Wall: this enables all the warnings about constructions that some users
#            consider questionable, and that are easy to avoid (or modify to
#            prevent the warning), even in conjunction with macros.
#     -Wextra: this enables some extra warning flags that are not enabled by -Wall
#     -Wuninitialized: warn if an automatic variable is used without first being
#                      initialized.
#     -Wfloat-equal: warn if floating-point values are used in equality
#                    comparisons.
#    -Wundef: warn if an undefined identifier is evaluated in an #if directive.
#    -Wpointer-arith: Warn about anything that depends on the “size of” a
#                     function type or of void.
#    -Wconversion: warn for implicit conversions that may alter a value.
# Debugging options:
#     -ggdb: produce debugging information for use by GDB.
# Optimization:
#     -03: optimize yet more.

CFLAGS = -Wall -Wextra -Wuninitialized -Wfloat-equal -Wundef \
	-Wpointer-arith -Wconversion -ggdb -O3 -Werror

CC = gcc
INCDIR = include
LDFLAGS = -Llib/
LDLIBS = -lpthread -lrt -larray

.PHONY: all lib server client

all: lib server client

lib:
	$(CC) -I$(INCDIR) $(CFLAGS) -fpic -shared -o lib/libarray.so lib/array.c

server:
	$(CC) -I$(INCDIR) $(LDFLAGS) $(LDLIBS) $(CFLAGS) src/server.c -o server

client:
	$(CC) -I$(INCDIR) $(LDFLAGS) $(LDLIBS) $(CFLAGS) src/client.c -o client

clean:
	rm server client lib/libarray.so
