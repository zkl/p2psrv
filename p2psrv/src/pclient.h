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
	pclient_proc proc_msg;
	pthread_mutex_t inmp;
	pthread_mutex_t outmp;
	void * ptr;
}pclient_t;

typedef int (*pclient_handle_msg)(pserver_t * server, pclient_t * client);

typedef struct _pclient_data
{
	queue_t inqueue;
	queue_t outqueue;
	pclient_handle_msg handle_msg;
}pclient_data_t;

extern pclient_t * pclient_create(int fd);
extern void pclient_destroy(pclient_t * client);
extern int pclient_proc_message(pserver_t * server, pclient_t * client);

void client_create();
void client_delete();

void client_time_out();
void client_data_arrived();
void client_data_sendto();

#endif
