#include <stdio.h>
#include "flight.h"

void flight_create(flight_t *flight, int id, double latitude, double longitude)
{
    flight->id = id;
    flight->latitude = latitude;
    flight->longitude = longitude;
    flight->thread = 0;
}

void flight_update_coordinates(flight_t *flight)
{
    flight->latitude += 0.01;
    flight->longitude += 0.01;
}

void flight_send_message(const flight_t *flight)
{
    printf("[F-%d] Has reached new coordinates: (%.2f, %.2f).\n", flight->id, flight->latitude, flight->longitude);
}