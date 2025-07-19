#include <stdint.h>
#include <string.h>
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
    // Generator (source: mode-s.org)
    uint32_t generator = 0x1FFFFA09;

    // Fill PI with 0x00
    message[11] = 0x00;
    message[12] = 0x00;
    message[13] = 0x00;

    // Make a copy of original message to manipulate
    uint8_t data[14];
    memcpy(data, message, 14);

    // CRC calculation
    for (int i = 0; i <= 112-25; i++) {
        int byte_pos = i / 8;
        int bit_pos = 7 - (i % 8);
        if (data[byte_pos] & (1 << bit_pos)) {
            for (int g = 0; g < 25; g++) {
                int bx = (i + g) / 8;
                int bx_bit = 7 - ((i + g) % 8);
                if (generator & (1 << (24 - g))) {
                    data[bx] ^= (1 << bx_bit);
                }
            }
        }
    }

    // Set the PI bits in the message
    message[11] = data[11];
    message[12] = data[12];
    message[13] = data[13];
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