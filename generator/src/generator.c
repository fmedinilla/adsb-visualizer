#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "generator.h"
#include "flight.h"

void *flight_simulation(void *arg)
{
    flight_t *flight = (flight_t *)arg;

    while (1) {
        // update coordinates for simulation
        flight_update_coordinates(flight);

        // send message
        flight_send_message(flight);

        // wait for 100 ms
        usleep(100000);
    }
    
    return NULL;
}

void generator_start(const int NUM_FLIGHTS)
{
    flight_t flights[NUM_FLIGHTS];

    // create flights with initial data
    int ids[] = {101, 102, 103};
    double latitudes[] = {4.5, 41.0, -20.0};
    double longitudes[] = {-1.0, 3.4 , 1.0};

    for (int i = 0; i < NUM_FLIGHTS; i++) {
        flight_create(&flights[i], ids[i], latitudes[i], longitudes[i]);
    }

    // create threads for each flight
    for (int i = 0; i < NUM_FLIGHTS; i++) {
        pthread_create(&flights[i].thread, NULL, flight_simulation, (void *)&flights[i]);
    }

    // wait for all threads to finish
    for (int i = 0; i < NUM_FLIGHTS; i++) {
        pthread_join(flights[i].thread, NULL);
    }
}