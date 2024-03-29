/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

int
_init_1 (void  *argp, void *result, struct svc_req *rqstp)
{
	return (init_1_svc(result, rqstp));
}

int
_insert_1 (insert_1_argument *argp, void *result, struct svc_req *rqstp)
{
	return (insert_1_svc(argp->sender, argp->receiver, argp->id, argp->msg, argp->md5, result, rqstp));
}

int
_msg_n_1 (char * *argp, void *result, struct svc_req *rqstp)
{
	return (msg_n_1_svc(*argp, result, rqstp));
}

int
_query_1 (query_1_argument *argp, void *result, struct svc_req *rqstp)
{
	return (query_1_svc(argp->user, argp->id, result, rqstp));
}

static void
msg_store_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		insert_1_argument insert_1_arg;
		char *msg_n_1_arg;
		query_1_argument query_1_arg;
	} argument;
	union {
		int init_1_res;
		int insert_1_res;
		int msg_n_1_res;
		query_msg query_1_res;
	} result;
	bool_t retval;
	xdrproc_t _xdr_argument, _xdr_result;
	bool_t (*local)(char *, void *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case INIT:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (bool_t (*) (char *, void *,  struct svc_req *))_init_1;
		break;

	case INSERT:
		_xdr_argument = (xdrproc_t) xdr_insert_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (bool_t (*) (char *, void *,  struct svc_req *))_insert_1;
		break;

	case MSG_N:
		_xdr_argument = (xdrproc_t) xdr_wrapstring;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (bool_t (*) (char *, void *,  struct svc_req *))_msg_n_1;
		break;

	case QUERY:
		_xdr_argument = (xdrproc_t) xdr_query_1_argument;
		_xdr_result = (xdrproc_t) xdr_query_msg;
		local = (bool_t (*) (char *, void *,  struct svc_req *))_query_1;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	retval = (bool_t) (*local)((char *)&argument, (void *)&result, rqstp);
	if (retval > 0 && !svc_sendreply(transp, (xdrproc_t) _xdr_result, (char *)&result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	if (!msg_store_1_freeresult (transp, _xdr_result, (caddr_t) &result))
		fprintf (stderr, "%s", "unable to free results");

	return;
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (MSG_STORE, MSG_STORE_V);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, MSG_STORE, MSG_STORE_V, msg_store_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (MSG_STORE, MSG_STORE_V, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, MSG_STORE, MSG_STORE_V, msg_store_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (MSG_STORE, MSG_STORE_V, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
