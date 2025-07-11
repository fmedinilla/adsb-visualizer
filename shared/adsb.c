#include <stdio.h>
#include "adsb.h"

void adsb_create_frame(adsb_frame_t *frame)
{
    frame->DF = 0b10001;    // Set Downlink Format
    frame->CA = 0b101;      // Set Transponder Capacity
    frame->ICAO = 0;        // Initialize ICAO address to 0
    frame->ME = 0;          // Initialize Message to 0
    frame->PI = 0;          // Initialize Parity Information to 0
}

void adsb_set_icao(adsb_frame_t *frame, unsigned int icao)
{
    frame->ICAO = icao;
}

void adsb_set_message(adsb_frame_t *frame, adsb_message_t message)
{
    frame->ME = (uint64_t)message.type << 51;
    frame->ME |= message.content;
}

void adsb_pack_frame(const adsb_frame_t *frame, uint8_t *packed)
{
     // Empaquetar DF (5), CA (3), ICAO (24), y parte alta de ME (56)
    uint64_t header = ((uint64_t)frame->DF << 59) |
                      ((uint64_t)frame->CA << 56) |
                      ((uint64_t)frame->ICAO << 32) |
                      (frame->ME >> 24); // Parte alta de ME

    uint32_t me_low = (uint32_t)(frame->ME & 0xFFFFFF); // 24 bits bajos de ME

    // Empaquetar PI (24 bits)
    uint32_t pi = frame->PI & 0xFFFFFF;

    // Copiar header (64 bits) a packed[0..7]
    for (int i = 0; i < 8; ++i)
        packed[i] = (header >> (56 - i * 8)) & 0xFF;

    // Copiar ME parte baja (3 bytes) a packed[8..10]
    for (int i = 0; i < 3; ++i)
        packed[8 + i] = (me_low >> (16 - i * 8)) & 0xFF;

    // Copiar PI (3 bytes) a packed[11..13]
    for (int i = 0; i < 3; ++i)
        packed[11 + i] = (pi >> (16 - i * 8)) & 0xFF;
}

void adsb_print_hex(const adsb_frame_t *frame)
{
    uint8_t packed[14] = {0};
    adsb_pack_frame(frame, packed);

    printf("Packed bytes (hex): ");
    for (int i = 0; i < 14; ++i)
        printf("%02X ", packed[i]);
    printf("\n");
}