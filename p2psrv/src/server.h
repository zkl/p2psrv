#ifndef SERVER_H
#define SERVER_H


#ifdef  DEBUG
#define DEBUG_SERVER_PRINT printf
#else
#define DEBUG_SERVER_PRINT
#endif

#include <netinet/in.h>
#include <sys/epoll.h>
#include "ptimer.h"
#include "pworker.h"

#define MAX_EPOLL_SIZE 1000

typedef struct _pserver
{
	int listen_fd;
	int listen_port;
	int client_num;
	int max_listen_num;
	int is_actived;
	int kdpfd;
	struct sockaddr_in addr;
	pworker_t worker;
	ptimer_t  timer;
}pserver_t;

typedef struct _pserver_data_t
{
	int fd;
	void * client;
}pserver_data_t;

void pserver_init(pserver_t * server, int max_listen, int port);
void pserver_exec(pserver_t * server);
void pserver_quit(pserver_t * server);
void pserver_destroy(pserver_t * server);
void pserver_outline(pserver_t * server, void * client);

#endif
