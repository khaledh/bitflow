/**
 * Programmable Interval Timer (PIT)
 */

#include "arch_x86/port.h"
#include "kernel/interrupt.h"
#include "kernel/scheduler.h"
#include "lib/util.h"
#include "device/console.h"
#include "device/pic.h"
#include "device/pit.h"

#define PIT_FREQUENCY 1193180
#define PIT_CH0_DATA  0x40
#define PIT_COMMAND   0x43

#define PIT_COUNTER_0   0b00000000
#define PIT_RW_LSB_MSB  0b00110000
#define PIT_MODE_SQUARE 0b00000110

#define TIMER_HZ 250

extern task_t* current_task;

static uint32_t ticks = 0;

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
    static int count = 0;
    static char msg[] = "________\0";

    ticks++;

    if ((ticks & 0xF) == 0) { // mod 16
        put_char(spinner[count++ % 4], (GRAY_DK << 4 | WHITE), 24, 0);
    }

    to_hex32(ticks, msg);
    put_str(msg, (GRAY_DK << 4 | WHITE), 24, 2);

    to_hex8(current_task->id, msg);
    put_str(msg, (GRAY_DK << 4 | WHITE), 24, 11);
}

void handle_interrupt(interrupt_frame_t* frame) {
    tick();
    schedule(READY);
}

/**
 * Install the timer IRQ handler at IRQ0.
 */
void pit_init() {
    set_frequency(TIMER_HZ);
    irq_install(0, handle_interrupt);
}
