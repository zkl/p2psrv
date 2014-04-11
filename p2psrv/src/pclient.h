#ifndef PCLIENT_H
#define PCLIENT_H

#define PCLIENT_DEBUG_OUT printf

#include <pthread.h>
#include "server.h"
#include "queue.h"

typedef struct _pclient
{
	int listen_fd;
	pserver_t * server;
	pthread_mutex_t m_mutex;
	queue_t * inqueue;
	queue_t * outqueue;
	void * ptr;
	void * sdata;
}pclient_t;


pclient_t * pclient_create(int fd, pserver_t * server, void * sdata);
void 	pclient_free(pclient_t * client);
void 	pclient_data_in (pclient_t * client);
void 	pclient_data_out(pclient_t * client);
void 	pclient_time_out(pclient_t * client);
void *	pclient_srv_data(pclient_t * client);

#endif
