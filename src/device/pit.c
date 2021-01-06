/**
 * Programmable Interval Timer (PIT)
 */

#include "../arch_x86/port.h"
#include "../kernel/interrupt.h"
#include "../kernel/task.h"
#include "console.h"
#include "pic.h"
#include "pit.h"

#define PIT_FREQUENCY 1193180
#define PIT_CH0_DATA  0x40
#define PIT_COMMAND   0x43

#define PIT_COUNTER_0   0b00000000
#define PIT_RW_LSB_MSB  0b00110000
#define PIT_MODE_SQUARE 0b00000110

#define TIMER_HZ 100

void set_frequency(uint16_t hz) {
    uint16_t divisor = PIT_FREQUENCY / hz;

    // send command
    port_out8(PIT_COMMAND, PIT_COUNTER_0 | PIT_RW_LSB_MSB | PIT_MODE_SQUARE);
    // send divisor
    port_out8(PIT_CH0_DATA, divisor & 0xff);
    port_out8(PIT_CH0_DATA, divisor >> 8);
}

void tick() {
    static char spinner[] = { '-', '\\', '|', '/' };
    static int counter = 0;
    static int tick = 0;
    if (tick++ % 5 == 0) {
        put_char(spinner[counter++ % 4], (GRAY_DK << 4 | WHITE), 24, 0);
    }
}

__attribute__ ((interrupt))
static void handle_interrupt(interrupt_frame_t* frame) {
    // ack interrupt
    irq_eoi(0);

    tick();
    schedule();
}

/**
 * Install the timer IRQ handler at IRQ0.
 */
void timer_init() {
    set_frequency(TIMER_HZ);
    irq_install(0, handle_interrupt);
}
