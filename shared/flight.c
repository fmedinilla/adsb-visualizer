#include <stdio.h>
#include <math.h>
#include "flight.h"
#include "adsb.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void flight_create(flight_t *flight, int ICAO, const char *callsign, double latitude, double longitude, int altitude, int speed, double track)
{
    flight->ICAO = ICAO;
    snprintf(flight->callsign, sizeof(flight->callsign), "%s", callsign);
    flight->latitude = latitude;
    flight->longitude = longitude;
    flight->altitude = altitude;
    flight->speed = speed;
    flight->track = track;
    flight->thread = 0;
}

void flight_update_coordinates(flight_t *flight)
{
    // const double EARTH_RADIUS = 6371000; // in meters
    const double DEG_TO_RAD = M_PI / 180.0;
    const double KNOTS_TO_MPS = 0.514444; // Conversion factor from knots to meters per second
    const double LATITUDE_TO_METERS = 111320; // Approximate conversion factor for latitude to meters
    const double LONGITUDE_TO_METERS = 111320 * cos(flight->latitude * DEG_TO_RAD); // Approximate conversion factor for longitude to meters at given latitude

    double velocity = flight->speed * KNOTS_TO_MPS; // Convert knots to m/s

    double distance = velocity * 1 * 60; // Distance traveled in 1 s

    double direction = flight->track * DEG_TO_RAD; // Convert track to radians

    double delta_lat = distance * cos(direction) / LATITUDE_TO_METERS;
    double delta_lon = distance * sin(direction) / LONGITUDE_TO_METERS;

    flight->latitude += delta_lat;
    flight->longitude += delta_lon;
}

void flight_get_message(flight_message_t message, flight_t *flight)
{
    // Header
    message[0] = 0x8D;
    
    // ICAO
    message[1] = (flight->ICAO >> 16) & 0xFF;
    message[2] = (flight->ICAO >> 8) & 0xFF;
    message[3] = flight->ICAO & 0xFF;

    // Message
    message[4] = 0x22;

    for (int i = 0; i < 8; i += 4) {
        unsigned int v = 0;
        for (int j = 0; j < 4; ++j) {
            v |= adsb_identificaction_get_charpos(flight->callsign[i + j]) << (18 - 6 * j);
        }
        message[5 + (i / 4) * 3] = (v >> 16) & 0xFF;
        message[6 + (i / 4) * 3] = (v >> 8) & 0xFF;
        message[7 + (i / 4) * 3] = v & 0xFF;
    }

    // Parity
    message[11] = 0x00;
    message[12] = 0x00;
    message[13] = 0x00;
}

void flight_send_message(flight_message_t message)
{
    for (int i = 0; i < 14; i++) {
        printf("%02X ", message[i]);
    }
    printf("\n");
}