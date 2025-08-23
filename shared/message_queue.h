#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <pthread.h>
#include "flight.h"

typedef struct _message_queue {
    flight_message_t messages[100];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} message_queue_t;

extern message_queue_t message_queue;

void queue_init(message_queue_t *q);
int enqueue_message(message_queue_t *q, flight_message_t *message);
int dequeue_message(message_queue_t *q, flight_message_t *message);

#endif // MESSAGE_QUEUE_H