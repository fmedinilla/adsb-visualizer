#include <pthread.h>
#include <string.h>
#include "message_queue.h"

message_queue_t message_queue;

void queue_init(message_queue_t *q)
{
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

int enqueue_message(message_queue_t *q, flight_message_t *message)
{
    pthread_mutex_lock(&q->mutex);
    if (q->count == 100) {
        pthread_mutex_unlock(&q->mutex);
        return -1; // Queue is full
    }
    memcpy(q->messages[q->rear], message, sizeof(flight_message_t));
    q->rear = (q->rear + 1) % 100;
    q->count++;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

int dequeue_message(message_queue_t *q, flight_message_t *message)
{
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    memcpy(message, &q->messages[q->front], sizeof(flight_message_t));
    q->front = (q->front + 1) % 100;
    q->count--;
    pthread_mutex_unlock(&q->mutex);
    return 0;
}