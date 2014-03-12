#ifndef PTIMER_H
#define PTIMER_H

#include <pthread.h>
#include "list.h"

typedef struct _ptimer
{
	int goon;
	list_t * events;
	pthread_t tid;
	pthread_mutex_t mp;
	pthread_cond_t cv;
}ptimer_t;

typedef void * (*worker_t)(void * data);

void ptimer_create(ptimer_t * timer);
void ptimer_append(ptimer_t * timer, worker_t worker, void * data,
	int delay);
void ptimer_quit(ptimer_t * timer);
void ptimer_destroy(ptimer_t * timer);

#endif
