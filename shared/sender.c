#include <pthread.h>
#include "sender.h"
#include "flight.h"
#include "message_queue.h"

void *sender_func(void *args)
{
    flight_message_t message;
    while(1) {
        dequeue_message(&message_queue, &message);
        flight_send_message(message);
    }
}

void start_sender(pthread_t *sender)
{
    pthread_create(sender, NULL, sender_func, NULL);
}