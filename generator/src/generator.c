#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "generator.h"
#include "flight.h"
#include "message_queue.h"

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

void start_generator(flight_data_t *flight_data, int num_flights, pthread_t *sender)
{
    flight_t flights[num_flights];
    queue_init(&message_queue);

    for (int i = 0; i < num_flights; i++) {
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

    // create threads for each flight
    for (int i = 0; i < num_flights; i++) {
        pthread_create(&flights[i].thread, NULL, flight_simulation, (void *)&flights[i]);
    }

    // wait for all threads to finish
    for (int i = 0; i < num_flights; i++) {
        pthread_join(flights[i].thread, NULL);
    }
    pthread_join(*sender, NULL);
}
