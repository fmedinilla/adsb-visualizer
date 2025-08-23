#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "flight.h"
#include "adsb.h"
#include "message_queue.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
    flight->message_type = IDENTIFICATION_MESSAGE;
    flight->last_message_even = 0;
}

void flight_update_coordinates(flight_t *flight)
{
    // Constants for calculations
    const double EARTH_RADIUS = 6371000;
    const double KNOTS_TO_MPS = 0.514444;
    const double DEG_TO_RAD = M_PI / 180.0;
    const double RAD_TO_DEG = (180.0 / M_PI);

    // Convert degrees to radians
    double lat_rad = flight->latitude * DEG_TO_RAD;
    double lon_rad = flight->longitude * DEG_TO_RAD;
    double theta = flight->track * DEG_TO_RAD;

    // Calculate distance
    double speed_mps = flight->speed * KNOTS_TO_MPS;
    double distance = speed_mps * 1;

    // Calculate new latitude
    double new_lat = asin(sin(lat_rad) * cos(distance / EARTH_RADIUS) +
                          cos(lat_rad) * sin(distance / EARTH_RADIUS) * cos(theta));
    
    // Calculate new longitude
    double new_lon = lon_rad + atan2(sin(theta) * sin(distance / EARTH_RADIUS) * cos(lat_rad),
                                      cos(distance / EARTH_RADIUS) - sin(lat_rad) * sin(new_lat));
    
    // Update flight coordinates
    flight->latitude = new_lat * RAD_TO_DEG;
    flight->longitude = new_lon * RAD_TO_DEG;
}

void flight_get_message(flight_message_t message, flight_t *flight)
{
    // DF (5) - CA (3) - ICAO (24) - ME (56) - PI (24)

    // Downlink Format
    adsb_set_df(message, 17);

    // Capability
    adsb_set_ca(message, 5);

    // ICAO
    adsb_set_icao(message, flight->ICAO);

    // Message
    switch (flight->message_type)
    {
    case IDENTIFICATION_MESSAGE:
        adsb_set_identification_me(message, flight);
        break;
    case POSITION_MESSAGE:
        adsb_set_position_me(message, flight);
        break;
    default:
        break;
    }
    
    // Parity Information
    adsb_set_pi(message);
}

void flight_send_message(flight_message_t message)
{
    for (int i = 0; i < 14; i++) {
        printf("%02X", message[i]);
    }
    printf("\n");
}