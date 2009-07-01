#ifndef __EVENT_QUEUE_H
#define __EVENT_QUEUE_H

struct queue_struct;
typedef struct queue_struct *queue_t;

int queue_empty (queue_t q);
int queue_full (queue_t q);
queue_t queue_create (int num_elements);
void queue_destroy (queue_t q);
void queue_make_empty (queue_t q);
void queue_enqueue (void *d, queue_t q);
void *queue_front (queue_t q);
void queue_dequeue (queue_t q);

#endif
