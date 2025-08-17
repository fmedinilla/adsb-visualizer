#include <stdint.h>
#include <string.h>
#include <math.h>
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

double positive_fmod(double a, double b) {
    double r = fmod(a, b);
    if (r < 0) r += b;
    return r;
}

uint32_t compute_lat_cpr(double latitude_deg, int is_even) {
    double dlat = is_even ? (360.0 / 60.0) : (360.0 / 59.0);
    double rlat = positive_fmod(latitude_deg, dlat);
    double lat_norm = rlat / dlat;
    uint32_t lat_cpr = (uint32_t)(lat_norm * 131072.0);
    return lat_cpr & 0x1FFFF;
}

int NL(double lat)
{
    if (lat < 0)
        lat = -lat;

    if (lat >= 87.0)
        return 1;

    const double pi = 3.14159265358979323846;
    const double a = 1 - cos(pi / 30);
    const double lat_rad = lat * pi / 180.0;

    double nl = 2 * pi / acos(1 - a / (cos(lat_rad) * cos(lat_rad)));

    return (int)floor(nl);
}

uint32_t compute_lon_cpr(double lon_deg, double lat_deg, int is_even) {
    int nl = NL(lat_deg);
    if (nl == 0) nl = 1;
    double dlon = is_even ? (360.0 / nl) : (360.0 / (nl - 1));
    double rlon = positive_fmod(lon_deg, dlon);
    double lon_norm = rlon / dlon;
    uint32_t lon_cpr = (uint32_t)(lon_norm * 131072.0);
    return lon_cpr & 0x1FFFF;
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
    // TC(5) - SS(2) - SAF(1) - ALT(12) - T(1) - F(1) - LAT_CPR(17) - LON_CPR(17)

    uint8_t is_even = flight->last_message_even ? 0 : 1;
    flight->last_message_even = !flight->last_message_even;

    int lat_cpr = compute_lat_cpr(flight->latitude, is_even);
    int lon_cpr = compute_lon_cpr(flight->longitude, flight->latitude, is_even);

    const double FT_TO_M = 0.3048;
    int altitude_m = (int)(flight->altitude * FT_TO_M);

    // byte 1: TC(0:4) - SS(0:1) - SAF(0:0)
    message[4] = 0xA0; // TC = 20 (altitude GNSS), SS = 0, SAF = 0

    // byte 2: ALT (0:7)
    message[5] = (altitude_m >> 4) & 0xFF;

    // byte 3: ALT (8:11) - T(0:0) - F(0:0) - LAT_CPR (0:1)
    uint8_t f = is_even ? 0 : 1; // F = 0 for even, 1 for odd
    message[6] = ((altitude_m >> 8) & 0x0F) << 4;
    message[6] |= (f << 2);
    message[6] |= (lat_cpr >> 15) & 0x03;

    // byte 4: LAT_CPR (2:9)
    message[7] = (lat_cpr >> 7) & 0xFF;

    // byte 5: LAT_CPR (10:16) - LON_CPR (0:0)
    message[8] = ((lat_cpr & 0x7F) << 1);
    message[8] |= (lon_cpr >> 16) & 0x01;

    // byte 6: LON_CPR (1:8)
    message[9] = (lon_cpr >> 8) & 0xFF;

    // byte 7: LON_CPR (9:16)
    message[10] = lon_cpr & 0xFF;
}