#ifndef FLIGHT_H
#define FLIGHT_H

#include <pthread.h>

typedef struct _flight {
    int ICAO;
    char callsign[9];
    double latitude;
    double longitude;
    int altitude;
    int speed;
    double track;
    pthread_t thread;
} flight_t;

void flight_create(flight_t *flight, int ICAO, const char *callsign, double latitude, double longitude, int altitude, int speed, double track);
void flight_update_coordinates(flight_t *flight);
void flight_send_message(const flight_t *flight);

#endif // FLIGHT_H