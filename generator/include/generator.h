#ifndef GENERATOR_H
#define GENERATOR_H

#include <pthread.h>
#include "flight.h"

int read_ini_data(flight_data_t **flight_data);
void start_generator(flight_data_t *flight_data, int num_flights, pthread_t *sender);

#endif // GENERATOR_H