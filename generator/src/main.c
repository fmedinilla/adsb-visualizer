#include <stdlib.h>
#include <pthread.h>
#include "generator.h"
#include "sender.h"

int main(void)
{
    flight_data_t *flight_data = NULL;
    const int NUM_FLIGHTS = read_ini_data(&flight_data);

    pthread_t sender;
    start_sender(&sender);

    start_generator(flight_data, NUM_FLIGHTS, &sender);

    return 0;
}