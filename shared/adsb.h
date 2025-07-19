#ifndef ADSB_H
#define ADSB_H

#include "flight.h"

void adsb_set_df(flight_message_t message, unsigned char df);
void adsb_set_ca(flight_message_t message, unsigned char ca);
void adsb_set_icao(flight_message_t message, int icao);
void adsb_set_pi(flight_message_t message);

void adsb_set_identification_me(flight_message_t message, flight_t *flight);
void adsb_set_position_me(flight_message_t message, flight_t *flight);

#endif // ADSB_H