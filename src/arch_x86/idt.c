/**
 * Interrupt Descriptor Table (IDT)
 */

#include "arch_x86/idt.h"

#define TASK_GATE         0b00101
#define INTERRUPT_GATE_32 0b01110
#define INTERRUPT_GATE_16 0b00110
#define TRAP_GATE_32      0b01111
#define TRAP_GATE_16      0b00111

typedef struct gate_desc {
    uint16_t offset_lo   : 16;
    uint16_t segment_sel : 16;
    uint16_t zero        :  8;
    uint16_t gate_type   :  5;
    uint16_t dpl         :  2;
    uint16_t present     :  1;
    uint16_t offset_hi   : 16;
} gate_desc_t;

static gate_desc_t idt[256];

static struct __attribute__((packed)) {
    uint16_t limit;
    void*    base;
} idt_desc;

void idt_init() {
    idt_desc.limit = (sizeof(gate_desc_t) * 256) - 1;
    idt_desc.base = idt;

    // load IDT descriptor into CPU
    asm("lidt %0" : : "m"(idt_desc));
}

void idt_set(uint8_t vector, isr_t handler) {
    idt[vector].segment_sel = 0x08; // code segment
    idt[vector].offset_lo = (uint32_t)handler & 0xffff;
    idt[vector].offset_hi = (uint32_t)handler >> 16;
    idt[vector].gate_type = INTERRUPT_GATE_32;
    idt[vector].dpl = 0; // privilege level
    idt[vector].present = 1;
}
