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
	worker->task_list = linked_list_create();
	worker->work_thread_list = linked_list_create();

	sem_init(&worker->sem, 0, 0);
	pthread_mutex_init(&worker->mp, 0);

	int i = 0;
	for(i=0; i<maxsize; i++)
	{
		pthread_t * tid = (pthread_t * )malloc(sizeof(pthread_t));
		linked_list_insert(worker->work_thread_list, 0, tid);
		pthread_create(tid, 0, &pworker_routine, worker);
	}
}

void pworker_remove(pworker_t * worker, void * data)
{

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

		/* 获取最后一个任务 */
		linked_list_node_t * node = linked_list_last(worker->task_list);
		d = (pworker_data_t *)linked_list_data(node);

		linked_list_remove(worker->task_list, node);

		DEBUG_WORKER_PRINT("获得任务 执行任务 当前任务列表剩余任务 %d\n", 
			worker->task_list->size);

		pthread_mutex_unlock(&worker->mp);

		d->task(d->data);
		free(d);
	}
	DEBUG_WORKER_PRINT("工作线程已经退出\n");
	return 0;
}

void pworker_append(pworker_t * worker, task_t task, void * data)
{
	pworker_data_t * d = (pworker_data_t *)malloc(
		sizeof(pworker_data_t));

	d->task = task;
	d->data = data;

	pthread_mutex_lock(&worker->mp);
	/* 在首部插入一个节点 */
	linked_list_insert(worker->task_list, 0, d);
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
	linked_list_node_t * node = worker->work_thread_list->head;
	for(; node; node = linked_list_next(node))
	{
		pthread_join(*((pthread_t *)linked_list_data(node)), &status);
	}
}

void pworker_destroy(pworker_t * worker)
{
	pthread_mutex_destroy(&worker->mp);
	sem_destroy(&worker->sem);
	linked_list_node_t* node = worker->work_thread_list->head;
	for(; node; node = node->next)
	{
		free(linked_list_data(node));
	}

	node = worker->task_list->head;
	for(; node; node = node->next)
	{
		free(linked_list_data(node));
	}

	linked_list_free(worker->task_list);
	linked_list_free(worker->work_thread_list);
}
