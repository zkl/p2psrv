#ifndef PCLIENT_H
#define PCLIENT_H

#define PCLIENT_DEBUG_OUT printf

#include <pthread.h>
#include "server.h"
#include "queue.h"

typedef enum _pclient_event{
	PCLIENT_TIMER_EVENT,
	PCLIENT_IN_EVENT,
	PCLIENT_OUT_EVENT,
	PCLIENT_ERROR_EVENT,
}pclient_event_t;

/* 返回 0 成功*/
typedef int (*pclient_proc)(pserver_t * server, pclient_event_t event, 
	void * ptr);

typedef struct _pclient
{
	int listen_fd;
	pserver_t * server;
	pthread_mutex_t m_mutex;
	queue_t * inqueue;
	queue_t * outqueue;
	void * ptr;
}pclient_t;


extern void pclient_destroy(pclient_t * client);
extern int pclient_proc_message(pserver_t * server, pclient_t * client);


pclient_t * pclient_create(int fd, pserver_t * server);
void pclient_free(pclient_t * client);
void pclient_data_in (pclient_t * client);
void pclient_data_out(pclient_t * client);
void pclient_time_out(pclient_t * client);

#endif
