#include "adsb.h"

const char *characters = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ##### ###############0123456789######";

int adsb_identificaction_get_charpos(char c)
{
    for (int i = 0; i < 64; i++) {
        if (characters[i] == c) {
            return i;
        }
    }
    return -1; // Character not found
}

char adsb_identificaction_get_char(int pos)
{
    if (pos >= 0 && pos < 64) {
        return characters[pos];
    }
    return 0; // Invalid position
}

void adsb_set_df(flight_message_t message, unsigned char df)
{
    message[0] = df;
}

void adsb_set_ca(flight_message_t message, unsigned char ca)
{
    message[0] <<= 3;
    message[0] |= ca;
}

void adsb_set_icao(flight_message_t message, int icao)
{
    message[1] = (icao >> 16) & 0xFF;
    message[2] = (icao >> 8) & 0xFF;
    message[3] = icao & 0xFF;
}

void adsb_set_pi(flight_message_t message)
{
    message[11] = 0xA0;
    message[12] = 0xB0;
    message[13] = 0xC0;
}

void adsb_set_identification_me(flight_message_t message, flight_t *flight)
{
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
}

void adsb_set_position_me(flight_message_t message, flight_t *flight)
{
    message[4] = 0x20;
    message[5] = 0xAA;
    message[6] = 0xBB;
    message[7] = 0xCC;
    message[8] = 0xDD;
    message[9] = 0xEE;
    message[10] = 0xFF;
}