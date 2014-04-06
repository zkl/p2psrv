#include <malloc.h>
#include "log.h"
#include "pclient.h"

pclient_t * pclient_create(int fd, pserver_t * server)
{
	pclient_t * client = (pclient_t *)malloc(sizeof(pclient_t));

	client->listen_fd = fd;
	client->server = server;
	
	pthread_mutex_init(&client->m_mutex, 0);
	queue_init(&client->inqueue, 8192);
	queue_init(&client->outqueue, 8192);

	return client;
}

void pclient_free(pclient_t * client)
{
	pthread_mutex_destroy(&client->m_mutex);
	queue_destroy(&client->inqueue);
	queue_destroy(&client->outqueue);
	free(client);
}
