/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

#include "p3.h"

#define ITER 100
int length[11] = {10, 100, 200, 500, 1000, 2000, 5000, 10000, 15000, 20000, 100000};

int main (int argc, char *argv[]) {
	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}

	CLIENT *clnt;
	enum clnt_stat ret;
	int result, i, j;
	char *str;
	//struct timeval t1, t2;
	clock_t t1, t2;
	clnt = clnt_create (argv[1], STR_SND, STR_SND_V, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror (argv[1]);
		exit (1);
	}

	for (i = 0; i < 11; ++i) {
		str = malloc(length[i]);
		memset(str, 'a', length[i]);
		//gettimeofday(&t1, NULL);
		t1 = clock();
		for (j = 0; j < ITER; ++j) {
			ret = print_str_1(str, &result, clnt);
			if (ret != RPC_SUCCESS) clnt_perror (clnt, "call failed");
		}
		//gettimeofday(&t2, NULL);
		t2 = clock();
		free(str);
		//printf("%d: %ld\n", length[i], t2.tv_sec - t1.tv_sec);
		printf("%d: %f\n", length[i], (((float)(t2 - t1) / 1000000.0F ) * 1000));
	}

	clnt_destroy (clnt);
exit (0);
}
