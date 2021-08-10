#ifndef DRUM_H
#define DRUM_H

#include <stdint.h>


typedef struct drum_state {
    uint8_t state_0;
    uint8_t state_1;
    uint8_t state_2;
    uint8_t state_3;
    uint8_t state_4;
    uint8_t state_5;
    uint8_t state_6;
    uint8_t state_7;
} __packed drum_state;


#endif