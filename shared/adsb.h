#ifndef ADSB_H
#define ADSB_H

#include <stdint.h>
#include "flight.h"

const char *characters = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ##### ###############0123456789######";

int adsb_identificaction_get_charpos(char c);
char adsb_identificaction_get_char(int pos);

#endif // ADSB_H