#ifndef PCLIENT_H
#define PCLIENT_H

#define PCLIENT_DEBUG_OUT printf

#include <pthread.h>
#include "server.h"
#include "queue.h"

typedef struct _pclient_
{
	int listen_fd;
	void * ptr;
	void * sdata;
	queue_t * inqueue;
	queue_t * outqueue;
	pserver_t * server;
	pthread_mutex_t m_mutex;
}pclient_t;


pclient_t * pclient_create(int fd, pserver_t * server, void * sdata);
pserver_t * pclient_server(pclient_t * client);
void        pclient_free(pclient_t * client);
void        pclient_error(pclient_t * client, int errno);
void 	    pclient_data_in(pclient_t * client);
void 	    pclient_data_out(pclient_t * client);
void     	pclient_time_out(pclient_t * client);

#endif
