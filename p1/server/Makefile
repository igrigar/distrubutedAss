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

#CFLAGS = -Wall -Wextra -Wuninitialized -Wfloat-equal -Wundef \
#	-Wpointer-arith -Wunused -Wconversion -ggdb -O3
#-Werror

CFLAGS = -ggdb

CC = gcc
INCDIR = -Iinclude -Irpc
LDFLAGS = -Llib/
LDLIBS = -lpthread -llist

.PHONY: lib server

all: lib server storage_s monitor

lib:
	$(CC) $(INCDIR) $(CFLAGS) -fpic -shared -o lib/liblist.so lib/list.c

server:
	$(CC) $(INCDIR) $(LDFLAGS) $(LDLIBS) $(CFLAGS) -o server src/server.c rpc/storage_clnt.c rpc/storage_xdr.c

monitor:
	$(CC) $(INCDIR) $(CFLAGS) -o monitor src/monitor.c rpc/storage_clnt.c rpc/storage_xdr.c

storage_s:
	$(CC) $(INCDIR) $(CFLAGS) -o storage_s src/storage_server.c rpc/storage_svc.c rpc/storage_xdr.c
clean:
	rm server storage_s monitor lib/liblist.so
