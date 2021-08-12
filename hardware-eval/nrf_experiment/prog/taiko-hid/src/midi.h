#ifndef MIDI_H
#define MIDI_H

#include <zephyr.h>

#define MIDI_IS_STATUS(x)  (x & (1 << 7))
#define MIDI_IS_DATA(x)   !(x & (1 << 7))
#define MIDI_STATUS(x)    ((x & 0xF0) >> 4)
#define MIDI_INFO(x)       (x & 0x0F)
#define MIDI_STATUS(x)    ((x & 0xF0) >> 4)
#define MIDI_INFO(x)       (x & 0x0F)
#define MIDI_NOTE_ON        0x9
#define MIDI_NOTE_OFF       0x8

#define MIDI_STATE_IDLE     0x0
#define MIDI_STATE_NOTE1    0x1
#define MIDI_STATE_NOTE2    0x2

int midi_init();

typedef struct midi_message {
    void *fifo_reserved;
    uint8_t note;
    uint8_t velocity;
} midi_message;

midi_message *midi_read();
void midi_free(midi_message *message);

#endif