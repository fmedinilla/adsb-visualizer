#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "generator.h"
#include "flight.h"

typedef struct _flight_data {
    int ICAO;
    double lat;
    double lon;
    int alt;
    int speed;
    double track;
    char callsign[9];
} flight_data_t;

typedef struct _message_queue {
    flight_message_t messages[100];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} message_queue_t;

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

void *sender_func(void *args)
{
    flight_message_t message;
    while(1) {
        dequeue_message(&message_queue, &message);
        flight_send_message(message);
    }
}

void *flight_simulation(void *arg)
{
    flight_t *flight = (flight_t *)arg;

    while (1) {
        // update coordinates for simulation
        flight_update_coordinates(flight);

        // Get message
        flight_message_t message;
        flight_get_message(message, flight);

        // Change message type for next iteration
        if (flight->message_type == IDENTIFICATION_MESSAGE)
            flight->message_type = POSITION_MESSAGE;
        else
            flight->message_type = IDENTIFICATION_MESSAGE;

        // send message
        enqueue_message(&message_queue, &message);
        //flight_send_message(message);

        // wait for 100 ms
        usleep(100000);
    }
    
    return NULL;
}

int read_ini_data(flight_data_t **flight_data)
{
    FILE *file = fopen("ini_flights.txt", "r");

    if (!file) {
        printf("Error: Could not open ini_flights.txt\n");
        return 0;
    }

    int num = 1;
    fscanf(file, "%d", &num);

    *flight_data = realloc(*flight_data, num * sizeof(flight_data_t));

    for (int i = 0; i < num; i++) {
        fscanf(file, "%d %lf %lf %d %d %lf %s",
            &(*flight_data)[i].ICAO,
            &(*flight_data)[i].lat,
            &(*flight_data)[i].lon,
            &(*flight_data)[i].alt,
            &(*flight_data)[i].speed,
            &(*flight_data)[i].track,
            (*flight_data)[i].callsign
        );

        (*flight_data)[i].callsign[8] = '\0';
    }

    fclose(file);
    return num;  // Return the number of flights read
}

void generator_start()
{
    pthread_t sender;

    flight_data_t *flight_data = malloc(sizeof(flight_data_t));
    const int NUM_FLIGHTS = read_ini_data(&flight_data);

    flight_t flights[NUM_FLIGHTS];

    queue_init(&message_queue);

    pthread_create(&sender, NULL, sender_func, NULL);

    for (int i = 0; i < NUM_FLIGHTS; i++) {
        flight_create(&flights[i],
            flight_data[i].ICAO,
            flight_data[i].callsign,
            flight_data[i].lat,
            flight_data[i].lon,
            flight_data[i].alt,
            flight_data[i].speed,
            flight_data[i].track
        );
    }

    free(flight_data);

    // create threads for each flight
    for (int i = 0; i < NUM_FLIGHTS; i++) {
        pthread_create(&flights[i].thread, NULL, flight_simulation, (void *)&flights[i]);
    }

    // wait for all threads to finish
    for (int i = 0; i < NUM_FLIGHTS; i++) {
        pthread_join(flights[i].thread, NULL);
    }
    pthread_join(sender, NULL);
}