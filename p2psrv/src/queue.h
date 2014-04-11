#ifndef QUEUE_H
#define QUEUE_H

typedef struct _queue_
{
	int size;
	char * head;
	char * rear;
}queue_t;

queue_t * queue_create(int maxsize);
int queue_free(queue_t * q);

int queue_enqueue(queue_t * q, int size);
int queue_dequeue(queue_t * q, int size);

int queue_size(queue_t * q);
int queue_left(queue_t * q);

char * queue_data(queue_t * q);
char * queue_last(queue_t * q);

#endif
