#include <malloc.h>
#include "log.h"
#include "pclient.h"

int pclient_proc_default(pserver_t * server, pclient_event_t event,
	void * ptr);

int pclient_handle_error(pserver_t * server, void * ptr);
int pclient_handle_in(pserver_t * server, void * ptr);
int pclient_handle_out(pserver_t * server, void * ptr);

pclient_t * pclient_create(int fd)
{
	pclient_t * client = (pclient_t *)malloc(sizeof(pclient_t));
	pclient_data_t * data = (pclient_data_t *)malloc(
		sizeof(pclient_data_t));

	client->listen_fd = fd;
	client->proc_msg = pclient_proc_default;
	client->ptr = data;
	
	data->handle_msg = pclient_proc_message;
	pthread_mutex_init(&client->inmp, 0);
	pthread_mutex_init(&client->outmp, 0);
	queue_init(&data->inqueue, 8192);
	queue_init(&data->outqueue, 8192);

	return client;
}

void pclient_destroy(pclient_t * client)
{
	pclient_data_t * data = (pclient_data_t *)client->ptr;
	
	pthread_mutex_destroy(&client->inmp);
	pthread_mutex_destroy(&client->outmp);
	queue_destroy(&data->inqueue);
	queue_destroy(&data->outqueue);
	free(data);
	free(client);
}

int pclient_proc_default(pserver_t * server, pclient_event_t event,
	void * ptr)
{
	switch(event)
	{
	case PCLIENT_OUT_EVENT:
		return pclient_handle_out(server, ptr);
	case PCLIENT_IN_EVENT:
		return pclient_handle_in(server, ptr);
	case PCLIENT_ERROR_EVENT:
		return pclient_handle_error(server, ptr);
	}
	return 0;
}

int pclient_handle_error(pserver_t * server, void * ptr)
{
	LOG_ERROR("client handle error.");
	pclient_t * client = (pclient_t *)ptr;
	return 1;
}

int pclient_handle_in(pserver_t * server, void * ptr)
{
	pclient_t * client = (pclient_t *)ptr;
	pclient_data_t * data = (pclient_data_t * )client->ptr;

	int lift_size;
	int recv_bytes;
	char * buf;

	pthread_mutex_lock(&client->inmp);
	while(1)
	{
		lift_size = queue_lift(&data->inqueue);

		if(lift_size > 0) 
		{
			buf = queue_last(&data->inqueue);
			recv_bytes = recv(client->listen_fd, buf, 
				queue_lift(&data->inqueue), 0);

			if(recv_bytes <= 0)
			{
				pthread_mutex_unlock(&client->inmp);
				return 1;
			}

			if(recv_bytes < 0)
				break;

			queue_enqueue(&data->inqueue, recv_bytes);
		}

		if(data->handle_msg(server, client) == 0)
			break;
	}

	pthread_mutex_unlock(&client->inmp);
	return 0;
}

int pclient_handle_out(pserver_t * server, void * ptr)
{
	pclient_t * client = (pclient_t *)ptr;
	pclient_data_t * data = (pclient_data_t * )client->ptr;

	int send_bytes;
	int size;
	char * buf;

	pthread_mutex_lock(&client->outmp);
	while(1)
	{
		size = queue_size(&data->outqueue);
		if(size <= 0)
			break;

		buf = queue_data(&data->outqueue);
		send_bytes = send(client->listen_fd, buf, size, 0);

		if(send_bytes <= 0)
		{
			log_error("send bytes == 0");
			pthread_mutex_unlock(&client->outmp);
			return 1;
		}

		if(send_bytes < 0)
			break;

		queue_dequeue(&data->outqueue, send_bytes);
	}
	pthread_mutex_unlock(&client->outmp);
	return 0;
}
