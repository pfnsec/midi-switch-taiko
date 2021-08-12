
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>

#include "buttons.h"
#include "midi.h"
#include "notes.h"


static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
							      {0});
static struct gpio_callback button_cb_data;


void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

void buttons_init() {

	int ret;

	if (!device_is_ready(button.port)) {
		printk("Error: button device %s is not ready\n",
		       button.port->name);
		return;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button.port->name, button.pin);
		return;
	}

	ret = gpio_pin_interrupt_configure_dt(&button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button.port->name, button.pin);
		return;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);
	printk("Set up button at %s pin %d\n", button.port->name, button.pin);
}


#define DRUM_COOLDOWN 10


static uint8_t drum_cooldown[8];

void update_cooldown(uint8_t note) {

    switch(note) {
        case SNARE:
            drum_cooldown[SNARE_I] = DRUM_COOLDOWN;
            break;
        case HI_TOM:
            drum_cooldown[HI_TOM_I] = DRUM_COOLDOWN;
            break;
        case MID_TOM:
            drum_cooldown[MID_TOM_I] = DRUM_COOLDOWN;
            break;
        case LOW_TOM:
            drum_cooldown[LOW_TOM_I] = DRUM_COOLDOWN;
            break;
        case CRASH:
            drum_cooldown[CRASH_I] = DRUM_COOLDOWN;
            break;
        case RIDE:
            drum_cooldown[RIDE_I] = DRUM_COOLDOWN;
            break;
        case HI_HAT:
            drum_cooldown[HI_HAT_I] = DRUM_COOLDOWN;
            break;
        default:
            break;
    }
}

void read_buttons(drum_state *state) {
    midi_message *msg;
    msg = midi_read();

    state->state_0 = 0;
    state->state_1 = 0;
    state->state_2 = 15;
    state->state_3 = 128;
    state->state_4 = 128;
    state->state_5 = 128;
    state->state_6 = 128;
    state->state_7 = 0;

    if(msg != NULL) {
        //printk("NOTE %d, VEL %d\n", msg->note, msg->velocity);
        if(msg->velocity > 0) {
            update_cooldown(msg->note);
        }
        midi_free(msg);
    }

    if(drum_cooldown[SNARE_I] > 0) {
        state->state_1 |= 4;
    }

    if(drum_cooldown[HI_TOM_I] > 0) {
        state->state_0 |= 64;
    }

    if(drum_cooldown[MID_TOM_I] > 0) {
        state->state_0 |= 128;
    }

    if(drum_cooldown[LOW_TOM_I] > 0) {
        state->state_1 |= 8;
    }

    if(drum_cooldown[CRASH_I] > 0) {
        state->state_0 |= 4;
    }

    if(drum_cooldown[RIDE_I] > 0) {
        state->state_0 |= 48;
    }

    if(drum_cooldown[HI_HAT_I] > 0) {
        state->state_1 |= 2;
    }

    for(int i = 0; i <= DRUM_I_MAX; i++) {
        if(drum_cooldown[i] > 0) {
            drum_cooldown[i]--;
        }
    }
}