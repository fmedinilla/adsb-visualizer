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
    int ICAOS[] = {0xAAA001};
    double latitudes[] = {37.4455675};
    double longitudes[] = {-5.9340945};
    int altitudes[] = {10000};
    int speeds[] = {300};
    double tracks[] = {40.6};
    char *callsigns[] = {"ABCD1001"};

    for (int i = 0; i < NUM_FLIGHTS; i++) {
        flight_create(&flights[i], ICAOS[i], callsigns[i], latitudes[i], longitudes[i], altitudes[i], speeds[i], tracks[i]);
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