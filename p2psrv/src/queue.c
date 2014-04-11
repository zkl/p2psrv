#include <string.h>
#include <malloc.h>
#include "queue.h"

queue_t * queue_create(int maxsize)
{
	queue_t * q = (queue_t *)malloc(sizeof(queue_t));
	q->size = maxsize;
	q->head = (char *)malloc(sizeof(char) * maxsize + sizeof(int));
	q->rear = q->head;

	return q;
}

int queue_free(queue_t * q)
{
	free(q->head);
	q->size = 0;
	q->head = 0;
	q->rear = 0;

	free(q);
	return 0;
}

int queue_enqueue(queue_t * q, int size)
{
	if(queue_left(q) < size || size <= 0)
		return -1;

	q->rear += size;
	*(q->rear) = 0;

	return size;
}

int queue_dequeue(queue_t * q, int size)
{
	if(queue_size(q) < size || size <= 0)
		return -1;

	memcpy(q->head, q->head + size, (q->rear - q->head) - size);
	q->rear -= size;
	*(q->rear) = 0; 
	return size;
}

int queue_size(queue_t * q)
{
	return q->rear - q->head;
}

int queue_left(queue_t * q)
{
	return q->size - (q->rear - q->head);
}

char * queue_data(queue_t * q)
{
	return q->head;
}

char * queue_last(queue_t * q)
{
	return q->rear;
}
