#ifndef FLIGHT_H
#define FLIGHT_H

#include <pthread.h>

typedef struct _flight {
    int id;
    double latitude;
    double longitude;
    pthread_t thread;
} flight_t;

void flight_create(flight_t *flight, int id, double latitude, double longitude);
void flight_update_coordinates(flight_t *flight);
void flight_send_message(const flight_t *flight);

#endif // FLIGHT_H