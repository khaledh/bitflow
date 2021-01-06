#include <stdint.h>
#include "../arch_x86/idt.h"
#include "../arch_x86/port.h"
#include "pic.h"

#define IRQ_BASE_VECTOR 0x20

#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0
#define PIC1_DATA    0x21
#define PIC2_DATA    0xA1

#define ICW1_ICW4    0x01  // ICW4 needed
#define ICW1_INIT    0x10  // initialization
#define ICW4_8086    0x01  // 8086/88 (MCS-80/85) mode
#define PIC_EOI      0x20  // End-of-interrupt command

/**
 * Remap PIC1 and PIC2 interrupt vectors to start at 32 and 40, respectively.
 */
static void remap_pic() {
    // ICW1 - begin initialization
    port_out8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    port_out8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    // ICW2 - remap interrupt vectors
    port_out8(PIC1_DATA, IRQ_BASE_VECTOR);
    port_out8(PIC2_DATA, IRQ_BASE_VECTOR + 8);

    // ICW3 - setup cascading
    port_out8(PIC1_DATA, 0b0100);  // slave PIC at IRQ2 (0000 0100)
    port_out8(PIC2_DATA, 0x0010);  // slave cascade identity (0000 0010)

    // ICW4 - set mode
    port_out8(PIC1_DATA, ICW4_8086);
    port_out8(PIC2_DATA, ICW4_8086);
}

/**
 * Disable a specific IRQ line.
 */
void irq_disable(uint8_t irq_no) {
    if (irq_no > 15) {
        return;
    }

    uint16_t port = PIC1_DATA;
    if (irq_no >= 8) {
        port = PIC2_DATA;
        irq_no -= 8;
    }

    uint8_t mask = port_in8(port) | (1 << irq_no);
    port_out8(port, mask);
}

/**
 * Enable a specific IRQ line.
 */
void irq_enable(uint8_t irq_no) {
    if (irq_no > 15) {
        return;
    }

    uint16_t port = PIC1_DATA;
    if (irq_no >= 8) {
        port = PIC2_DATA;
        irq_no -= 8;
    }

    uint8_t mask = port_in8(port) & ~(1 << irq_no);
    port_out8(port, mask);
}

/**
 * Disable all IRQs.
 */
void irq_disable_all() {
    // mask interrupts
    port_out8(PIC1_DATA, 0xff);
    port_out8(PIC2_DATA, 0xff);
}

/**
 * Enable all IRQs.
 */
void irq_enable_all() {
    // unmask interrupts
    port_out8(PIC1_DATA, 0x00);
    port_out8(PIC2_DATA, 0x00);
}

/**
 * Initialize PICs.
 */
void irq_init() {
    remap_pic();
    irq_disable_all();
}

/**
 * Install an IRQ handler.
 */
void irq_install(uint8_t irq_no, interrupt_handler_t irq_handler) {
    idt_set(IRQ_BASE_VECTOR + irq_no, irq_handler);
    irq_enable(irq_no);
}

/**
 * Send End of Interrupt to PICs.
 */
void irq_eoi(uint8_t irq_no) {
    if (irq_no >= 8) {
        port_out8(PIC2_COMMAND, PIC_EOI);
    }
    port_out8(PIC1_COMMAND, PIC_EOI);
}
