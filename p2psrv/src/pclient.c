#include <malloc.h>
#include "log.h"
#include "pclient.h"

pclient_t * pclient_create(int fd, pserver_t * server, void * sdata)
{
	pclient_t * client = (pclient_t *)malloc(sizeof(pclient_t));

	client->listen_fd = fd;
	client->server = server;
	client->sdata = sdata;
	
	pthread_mutex_init(&client->m_mutex, 0);
	client->inqueue = queue_create(8192);
	client->outqueue= queue_create(8192);

	return client;
}

void pclient_free(pclient_t * client)
{
	pthread_mutex_destroy(&client->m_mutex);
	queue_free(client->inqueue);
	queue_free(client->outqueue);
	free(client);
}

void pclient_data_in(pclient_t * client)
{
	
	int num = recv(client->listen_fd, queue_last(client->inqueue),
		queue_left(client->inqueue), 0);

	queue_enqueue(client->inqueue, num);

	LOG_DEBUG("%d %d data in", num, queue_size(client->inqueue));
}

void pclient_data_out(pclient_t * client)
{
	int num = send(client->listen_fd, queue_data(client->outqueue),
		queue_size(client->outqueue), 0);

	queue_dequeue(client->outqueue, num);

	LOG_DEBUG("%X data out",client);
}

void pclient_time_out(pclient_t * client)
{
	LOG_DEBUG("%X time out", client);
}

void * pclient_srv_data(pclient_t * client)
{
	return client->sdata;
}
