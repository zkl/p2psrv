#include <stdio.h>
#include <malloc.h>
#include "list.h"
#include "ptimer.h"


typedef struct  _ptimer_timer_
{
	unsigned int delay;
	worker_t worker;
	void * data;
}ptimer_timer_t;

void * ptimer_routine(void * arg);

void ptimer_create(ptimer_t * timer)
{
	timer->goon = 1;
	timer->events = linked_list_create();
	pthread_mutex_init(&timer->mp, 0);
	pthread_cond_init (&timer->cv, 0);
	pthread_create(&timer->tid, 0, &ptimer_routine, timer);
}

void ptimer_clear (ptimer_t * timer, void * data)
{
	pthread_mutex_lock(&timer->mp);
	linked_list_node_t * node = linked_list_first(timer->events);
	while(node)
	{
		ptimer_timer_t * ev = 
			(ptimer_timer_t *)linked_list_data(node);
		if(ev->data == data)
		{
			free(ev);
			node = linked_list_remove(timer->events, node);
		}
		else
		{
			node = linked_list_next(node);
		}
	}
	pthread_mutex_unlock(&timer->mp);
}

void * ptimer_routine(void * arg)
{
	ptimer_t * timer = (ptimer_t *)arg;

	while(1)
	{
		pthread_mutex_lock(&timer->mp);
		int size = linked_list_size(timer->events);
		if(size <= 0)
		{
			pthread_mutex_unlock(&timer->mp);
			pthread_cond_wait(&timer->cv, &timer->mp);
		}

		if(timer->goon <= 0)
			break;

		linked_list_t  * list = timer->events;

		linked_list_node_t  * node;
		for(node = linked_list_first(list); node; )
		{
			ptimer_timer_t * ev = 
				(ptimer_timer_t *)linked_list_data(node);

			if(ev->delay == 0)
			{
				ev->worker(ev->data);

				free(ev);
				node = linked_list_remove(list, node);
			}
			else
			{
				ev->delay--;
				node = linked_list_next(node);
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
	linked_list_insert(timer->events, 0, ev);
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
	linked_list_free(timer->events);
	pthread_mutex_destroy(&timer->mp);
	pthread_cond_destroy(&timer->cv);
}
