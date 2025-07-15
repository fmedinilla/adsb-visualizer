#include <stdio.h>
#include "adsb.h"

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