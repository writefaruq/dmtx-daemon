#include "event-queue.h"
#include <stdlib.h>

struct queue_struct
{
	int capacity;
	int front;
	int rear;
	int size;
	void **array;
};

int
queue_empty (queue_t q)
{
	return q->size == 0;
}

int queue_full (queue_t q)
{
	return q->size == q->capacity;
}

queue_t queue_create (int num_elements)
{
	queue_t q;

	q = malloc (sizeof (struct queue_struct));

	if (q == NULL)
		exit (-1);

	q->array = malloc(sizeof (void *) * num_elements);

	if (q->array == NULL)
		exit (-1);

	q->capacity = num_elements;

	queue_make_empty (q);

	return q;
}

void queue_destroy (queue_t q)
{
	if (q != NULL)
	{
		if (q->array)
			free (q->array);

		free (q);
	}
}

void queue_make_empty (queue_t q)
{
	q->size = 0;
	q->front = 1;
	q->rear = 0;
}

static int next_position (int v, queue_t q)
{
	if (++v == q->capacity) {
		v = 0;
	}

	return v;
}

void queue_enqueue (void *d, queue_t q)
{
	if (queue_full (q))
	{
		return;
	}

	q->size++;
	q->rear = next_position (q->rear, q);
	q->array[q->rear] = d;
}

void *queue_front (queue_t q)
{
	if (!queue_empty(q))
		return q->array [q->front];

	return NULL;
}

void queue_dequeue (queue_t q)
{
	if (!queue_empty (q))
	{
		q->size--;
		q->front = next_position (q->front, q);
	}
}
