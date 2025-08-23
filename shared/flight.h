#ifndef FLIGHT_H
#define FLIGHT_H

#include <pthread.h>
#include <stdint.h>

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
    uint8_t last_message_even;
} flight_t;

typedef struct _flight_data {
    int ICAO;
    double lat;
    double lon;
    int alt;
    int speed;
    double track;
    char callsign[9];
} flight_data_t;

typedef unsigned char flight_message_t[14];

void *flight_simulation(void *arg);
void flight_create(flight_t *flight, int ICAO, const char *callsign, double latitude, double longitude, int altitude, int speed, double track);
void flight_update_coordinates(flight_t *flight);
void flight_get_message(flight_message_t message, flight_t *flight);
void flight_send_message(flight_message_t message);

#endif // FLIGHT_H