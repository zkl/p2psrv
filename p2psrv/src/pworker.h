#ifndef WORKER_H
#define WORKER_H

#include <pthread.h>
#include <semaphore.h>
#include "list.h"

#ifdef  DEBUG
#define DEBUG_WORKER_PRINT printf
#else
#define DEBUG_WORKER_PRINT 
#endif

typedef struct _pworker
{
	int goon;
	list_t * task_list;
	list_t * work_thread_list;
	pthread_mutex_t mp;
	sem_t sem;
}pworker_t;

typedef void * (*task_t)(void * data);

void pworker_create(pworker_t * worker, int max_size);
void pworker_append(pworker_t * worker, task_t task, void * data);
void pworker_quit(pworker_t * worker);
void pworker_destroy(pworker_t * worker);

#endif
