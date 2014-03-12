#include <stdio.h>
#include <malloc.h>
#include "list.h"
#include "pworker.h"

typedef struct  _pworker_data
{
	task_t task;
	void * data;
}pworker_data_t;

void * pworker_routine(void * arg);

void pworker_create(pworker_t * worker, int maxsize)
{
	worker->goon = 1;
	worker->task_list = list_create();
	worker->work_thread_list = list_create();

	sem_init(&worker->sem, 0, 0);
	pthread_mutex_init(&worker->mp, 0);

	int i = 0;
	for(i=0; i<maxsize; i++)
	{
		pthread_t * tid = (pthread_t * )malloc(sizeof(pthread_t));
		list_append(worker->work_thread_list, tid);
		pthread_create(tid, 0, &pworker_routine, worker);
	}
}

void * pworker_routine(void * arg)
{
	pworker_t * worker = (pworker_t *)arg;
	while(worker->goon)
	{
		sem_wait(&worker->sem);
		if(worker->goon == 0)
			break;

		pworker_data_t * d;
		pthread_mutex_lock(&worker->mp);

		list_node_t * node = list_nodeat(worker->task_list, 0);
		d = (pworker_data_t *)node->data;

		list_delnde(worker->task_list, node);

		DEBUG_WORKER_PRINT("获得任务 执行任务 当前任务列表剩余任务 %d\n", 
			worker->task_list->size);

		pthread_mutex_unlock(&worker->mp);

		d->task(d->data);
		free(d);
	}
	DEBUG_WORKER_PRINT("工作线程已经退出\n");
}

void pworker_append(pworker_t * worker, task_t task, void * data)
{
	pworker_data_t * d = (pworker_data_t *)malloc(
		sizeof(pworker_data_t));

	d->task = task;
	d->data = data;

	pthread_mutex_lock(&worker->mp);
	list_append(worker->task_list, d);
	sem_post(&worker->sem);
	pthread_mutex_unlock(&worker->mp);
}

void pworker_quit(pworker_t * worker)
{
	pthread_mutex_lock(&worker->mp);
	worker->goon = 0;

	int i = 0;
	for(i=0; i<worker->work_thread_list->size; i++)
	{
		sem_post(&worker->sem);
	}

	pthread_mutex_unlock(&worker->mp);

	void * status;
	list_node_t * node = worker->work_thread_list->head;
	for(node ; node; node = node->next)
	{
		pthread_join(*((pthread_t *)node->data), &status);
	}
}

void pworker_destroy(pworker_t * worker)
{
	pthread_mutex_destroy(&worker->mp);
	sem_destroy(&worker->sem);
	list_node_t * node = worker->work_thread_list->head;
	for(node ; node; node = node->next)
	{
		free(node->data);
	}

	node = worker->task_list->head;
	for(node ; node; node = node->next)
	{
		free(node->data);
	}

	list_destroy(worker->task_list);
	list_destroy(worker->work_thread_list);
}
