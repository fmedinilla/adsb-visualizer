#ifndef ADSB_H
#define ADSB_H

#include <stdint.h>

typedef struct _adsb_frame {
    unsigned char DF : 5;       // Downlink Format (5 bits)
    unsigned char CA : 3;       // Transponder Capacity (3 bits)
    unsigned int ICAO : 24;     // ICAO address (24 bits)
    uint64_t ME: 56;            // Message (56 bits)
    uint32_t PI: 24;            // Parity Information (24 bits)
} adsb_frame_t;

typedef struct _adsb_message {
    unsigned char type : 5;     // Message type (5 bits)
    uint64_t content : 51;      // Message content (51 bits)
} adsb_message_t;

void adsb_create_frame(adsb_frame_t *frame);
void adsb_set_icao(adsb_frame_t *frame, unsigned int icao);
void adsb_set_message(adsb_frame_t *frame, adsb_message_t message);
void adsb_pack_frame(const adsb_frame_t *frame, uint8_t *packed);
void adsb_print_frame(const adsb_frame_t *frame);
void adsb_print_hex(const adsb_frame_t *frame);

#endif // ADSB_H