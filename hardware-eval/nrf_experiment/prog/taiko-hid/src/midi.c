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

static struct device *midi_uart_dev;

static midi_message latest_message;

static void midi_uart_isr(const struct device *unused, void *user_data) {
	ARG_UNUSED(unused);
	ARG_UNUSED(user_data);

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
        if(rx) {
            latest_message.chr = chr;
            k_fifo_put(&midi_fifo, &latest_message);
        }

        //LOG_DBG("read %d req %d", rx, len);
		printk("rx %d, chr %c\n", rx, chr);

	}
}

int midi_init() {

    /*
    #if DT_NODE_HAS_STATUS(MIDI_UART_NODE, okay)
        printk("I2S enabled in Zephyr DeviceTree \r\n");
    #else
        printk("I2S is disabled in DeviceTree\r\n");
    #endif
    */

	//midi_uart_dev = DEVICE_DT_GET(MIDI_UART_NODE);
	//midi_uart_dev = device_get_binding("UART_1");
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