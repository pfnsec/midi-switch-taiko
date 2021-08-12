#include <zephyr.h>
#include <arch/cpu.h>
#include <sys/byteorder.h>
#include <logging/log.h>
#include <sys/util.h>

#include <device.h>
#include <init.h>
#include <drivers/uart.h>

#include "midi.h"

static K_FIFO_DEFINE(midi_fifo);
static K_MEM_SLAB_DEFINE(midi_slab, sizeof(midi_message), 64, 4);


static struct device *midi_uart_dev;

static midi_message *latest_message = NULL;
static uint8_t state = MIDI_STATE_IDLE;

static void midi_uart_isr(const struct device *unused, void *user_data) {
	ARG_UNUSED(unused);
	ARG_UNUSED(user_data);

    if(latest_message == NULL) {
        k_mem_slab_alloc(&midi_slab, &latest_message, K_FOREVER);
        //printk("Realloc message... %p\n", latest_message);
    }

	if (!(uart_irq_rx_ready(midi_uart_dev) ||
	      uart_irq_tx_ready(midi_uart_dev))) {
		printk("spurious interrupt");
	}

	if (uart_irq_tx_ready(midi_uart_dev)) {
		printk("tx_ready");
	}

	if (uart_irq_rx_ready(midi_uart_dev)) {
		//printk("rx_ready");
        static uint8_t chr; 

        int rx = uart_fifo_read(midi_uart_dev, &chr, sizeof(chr));
        if(!rx) {
            return;
        }

        static uint8_t running_status;
        switch(state) {
        case MIDI_STATE_IDLE:
            if(MIDI_IS_STATUS(chr)){
                if(MIDI_STATUS(chr) == MIDI_NOTE_ON) {
                    state = MIDI_STATE_NOTE1;
                    running_status = chr;
                } else if(MIDI_STATUS(chr) == 0xF) {
                    // sysex, don't update running_status
                } else {
                    running_status = chr;
                }
            } else {
                if(MIDI_STATUS(running_status) == MIDI_NOTE_ON) {
                    latest_message->note = chr;
                    state = MIDI_STATE_NOTE2;
                }
                //printk("DATA in IDLE! %x\n", chr);
            }
            
            break;
        case MIDI_STATE_NOTE1:
            if(MIDI_IS_DATA(chr)){
                latest_message->note = chr;
                state = MIDI_STATE_NOTE2;
            } else {
                printk("STATUS in NOTE1! \n");
            }
            break;
        case MIDI_STATE_NOTE2:
            if(MIDI_IS_DATA(chr)){
                latest_message->velocity = chr;
                k_fifo_put(&midi_fifo, latest_message);
                k_mem_slab_alloc(&midi_slab, &latest_message, K_FOREVER);
                state = MIDI_STATE_IDLE;
            } else {
                printk("STATUS in NOTE2! \n");
            }
            break;
        default:
            printk("STATE CORRUPTED! \n");
            break;
        }
	}
}

int midi_init() {

    midi_uart_dev = device_get_binding(DT_LABEL(DT_NODELABEL(uart1)));
	if (midi_uart_dev == NULL) {
        printk("midi dev null!");
		return -1;
	}

	if (!device_is_ready(midi_uart_dev)) {
		printk("MIDI UART %s is not ready", midi_uart_dev->name);
		return -EINVAL;
	}

	uart_irq_rx_disable(midi_uart_dev);
	uart_irq_tx_disable(midi_uart_dev);

	uart_irq_callback_set(midi_uart_dev, midi_uart_isr);

	uart_irq_rx_enable(midi_uart_dev);

	return 0;
}

midi_message *midi_read() {
    return k_fifo_get(&midi_fifo, K_NO_WAIT);
}

void midi_free(midi_message *message) {
    k_mem_slab_free(&midi_slab, &message);
}