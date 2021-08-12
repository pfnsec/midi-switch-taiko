#ifndef MIDI_H
#define MIDI_H

#include <zephyr.h>


int midi_init();

typedef struct midi_message {
    void *fifo_reserved;
    uint8_t chr;
} midi_message;

midi_message *midi_read();

#endif