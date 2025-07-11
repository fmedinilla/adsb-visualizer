#ifndef FLIGHT_H
#define FLIGHT_H

#include <pthread.h>

typedef enum {
    IDENTIFICATION_MESSAGE,
    POSITION_MESSAGE,
} message_type_t;

typedef struct _flight {
    int ICAO;
    char callsign[9];
    double latitude;
    double longitude;
    int altitude;
    int speed;
    double track;
    pthread_t thread;

    message_type_t message_type;
} flight_t;

void flight_create(flight_t *flight, int ICAO, const char *callsign, double latitude, double longitude, int altitude, int speed, double track);
void flight_update_coordinates(flight_t *flight);
void flight_send_message(flight_t *flight);

#endif // FLIGHT_H