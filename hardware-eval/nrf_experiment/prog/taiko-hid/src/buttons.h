#ifndef BUTTONS_H
#define BUTTONS_H

#include <zephyr.h>
#include <device.h>
#include "drum.h"

#define SW0_NODE	DT_ALIAS(sw0)
#define SW1_NODE	DT_ALIAS(sw1)
#define SW2_NODE	DT_ALIAS(sw2)
#define SW3_NODE	DT_ALIAS(sw3)

#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(SW1_NODE, okay)
#error "Unsupported board: sw1 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(SW2_NODE, okay)
#error "Unsupported board: sw2 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(SW3_NODE, okay)
#error "Unsupported board: sw3 devicetree alias is not defined"
#endif

void buttons_init();

void read_buttons(drum_state *state);

#endif