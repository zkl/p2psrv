#include <stdio.h>
#include <malloc.h>
#include "list.h"
#include "ptimer.h"


typedef struct  _ptimer_timer
{
	unsigned int delay;
	worker_t worker;
	void * data;
}ptimer_timer_t;

void * ptimer_routine(void * arg);

void ptimer_create(ptimer_t * timer)
{
	timer->goon = 1;
	timer->events = list_create();
	pthread_mutex_init(&timer->mp, 0);
	pthread_cond_init (&timer->cv, 0);
	pthread_create(&timer->tid, 0, &ptimer_routine, timer);
}

void * ptimer_routine(void * arg)
{
	ptimer_t * timer = (ptimer_t *)arg;

	while(1)
	{
		pthread_mutex_lock(&timer->mp);
		int size = timer->events->size;
		if(size <= 0)
		{
			pthread_mutex_unlock(&timer->mp);
			pthread_cond_wait(&timer->cv, &timer->mp);
		}

		if(timer->goon <= 0)
			break;

		list_t * list = timer->events;

		list_node_t * node;
		for(node = list->head; node; )
		{
			ptimer_timer_t * ev = (ptimer_timer_t *)(node->data);
			if(ev->delay == 0)
			{
				ev->worker(ev->data);

				free(ev);
				node = list_delnde(list, node);
			}
			else
			{
				ev->delay--;
				node = node->next;
			}
		}

		pthread_mutex_unlock(&timer->mp);
		usleep(1000000);
	}
	return 0;
}

void ptimer_append(ptimer_t * timer, worker_t worke, 
	void * data, int delay)
{
	ptimer_timer_t * ev = (ptimer_timer_t *)malloc(
		sizeof(ptimer_timer_t));

	ev->delay = delay;
	ev->worker = worke;
	ev->data = data;

	pthread_mutex_lock(&timer->mp);
	list_append(timer->events, ev);
	pthread_cond_signal(&timer->cv);
	pthread_mutex_unlock(&timer->mp);
}

void ptimer_quit(ptimer_t * timer)
{
	pthread_mutex_lock(&timer->mp);
	timer->goon = 0;
	pthread_cond_signal(&timer->cv);
	pthread_mutex_unlock(&timer->mp);
}

void ptimer_destroy(ptimer_t * timer)
{
	void * status;
	pthread_join(timer->tid, status);
	list_destroy(timer->events);
	pthread_mutex_destroy(&timer->mp);
	pthread_cond_destroy(&timer->cv);
}
