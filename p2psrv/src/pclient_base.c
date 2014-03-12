#include <string.h>
#include "log.h"
#include "pclient.h"

int pclient_proc_message(pserver_t * server, pclient_t * client)
{
	pclient_data_t * pdata = (pclient_data_t *)client->ptr;

	log_debug("data size %d\n", queue_size(&pdata->inqueue));
	log_debug("缓冲区内容 \n%s", queue_data(&pdata->inqueue));
	
	char * buf = queue_data(&pdata->outqueue);
	strncpy(buf, queue_data(&pdata->inqueue), queue_size(&pdata->inqueue));
	queue_enqueue(&pdata->outqueue, queue_size(&pdata->inqueue));
	queue_dequeue(&pdata->inqueue, queue_size(&pdata->inqueue));
	
	return 0;
}

