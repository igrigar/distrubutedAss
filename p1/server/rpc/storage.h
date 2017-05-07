/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _STORAGE_H_RPCGEN
#define _STORAGE_H_RPCGEN

#include <rpc/rpc.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif


struct query_msg {
	char *msg;
	char *md5;
};
typedef struct query_msg query_msg;

struct insert_1_argument {
	char *sender;
	char *receiver;
	char *id;
	char *msg;
	char *md5;
};
typedef struct insert_1_argument insert_1_argument;

struct query_1_argument {
	char *user;
	char *id;
};
typedef struct query_1_argument query_1_argument;

#define MSG_STORE 99
#define MSG_STORE_V 1

#if defined(__STDC__) || defined(__cplusplus)
#define INIT 1
extern  enum clnt_stat init_1(int *, CLIENT *);
extern  bool_t init_1_svc(int *, struct svc_req *);
#define INSERT 2
extern  enum clnt_stat insert_1(char *, char *, char *, char *, char *, int *, CLIENT *);
extern  bool_t insert_1_svc(char *, char *, char *, char *, char *, int *, struct svc_req *);
#define MSG_N 3
extern  enum clnt_stat msg_n_1(char *, int *, CLIENT *);
extern  bool_t msg_n_1_svc(char *, int *, struct svc_req *);
#define QUERY 4
extern  enum clnt_stat query_1(char *, char *, query_msg *, CLIENT *);
extern  bool_t query_1_svc(char *, char *, query_msg *, struct svc_req *);
extern int msg_store_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define INIT 1
extern  enum clnt_stat init_1();
extern  bool_t init_1_svc();
#define INSERT 2
extern  enum clnt_stat insert_1();
extern  bool_t insert_1_svc();
#define MSG_N 3
extern  enum clnt_stat msg_n_1();
extern  bool_t msg_n_1_svc();
#define QUERY 4
extern  enum clnt_stat query_1();
extern  bool_t query_1_svc();
extern int msg_store_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_query_msg (XDR *, query_msg*);
extern  bool_t xdr_insert_1_argument (XDR *, insert_1_argument*);
extern  bool_t xdr_query_1_argument (XDR *, query_1_argument*);

#else /* K&R C */
extern bool_t xdr_query_msg ();
extern bool_t xdr_insert_1_argument ();
extern bool_t xdr_query_1_argument ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_STORAGE_H_RPCGEN */