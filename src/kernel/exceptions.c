/**
 * CPU Exceptions
 */

#include "../arch_x86/cpu.h"
#include "../arch_x86/idt.h"
#include "../device/console.h"
#include "../lib/util.h"

char* exception_msgs[] = {
    [0] = "[#DE] Divide Error",
    [1] = "[#DB] Debug Exception",
    [2] = "Non-Maskable Interrupt (NMI)",
    [3] = "[#BP] Breakpoint",
    [4] = "[#OF] Overflow",
    [5] = "[#BR] BOUND Range Exceeded",
    [6] = "[#UD] Invalid Opcode (Undefined Opcode)",
    [7] = "[#NM] Device Not Available (No Math Coprocessor)",
    [8] = "[#DF] Double Fault",
    [9] = "Coprocessor Segment Overrun",
    [10] = "[#TS] Invalid TSS",
    [11] = "[#NP] Segment Not Present",
    [12] = "[#SS] Stack-Segment Fault",
    [13] = "[#GP] General Protection",
    [14] = "[#PF] Page Fault",
    /* [15] is Intel reserved. */
    [16] = "[#MF] x87 FPU Floating-Point Error (Math Fault)",
    [17] = "[#AC] Alignment Check",
    [18] = "[#MC] Machine Check",
    [19] = "[#XM] SIMD Floating-Point Exception",
    [20] = "Virtualization Exception",
};


// Exceptions
extern isr_t isr00;
extern isr_t isr01;
extern isr_t isr02;
extern isr_t isr03;
extern isr_t isr04;
extern isr_t isr05;
extern isr_t isr06;
extern isr_t isr07;
extern isr_t isr08;
extern isr_t isr09;
extern isr_t isr10;
extern isr_t isr11;
extern isr_t isr12;
extern isr_t isr13;
extern isr_t isr14;
/* no isr15 */
extern isr_t isr16;
extern isr_t isr17;
extern isr_t isr18;
extern isr_t isr19;
extern isr_t isr20;

// IRQs
extern isr_t isr32;
extern isr_t isr33;
extern isr_t isr34;
extern isr_t isr35;
extern isr_t isr36;
extern isr_t isr37;
extern isr_t isr38;
extern isr_t isr39;
extern isr_t isr40;
extern isr_t isr41;
extern isr_t isr42;
extern isr_t isr43;
extern isr_t isr44;
extern isr_t isr45;
extern isr_t isr46;
extern isr_t isr47;


void exceptions_init() {
    idt_set(0, &isr00);
    idt_set(1, &isr01);
    idt_set(2, &isr02);
    idt_set(3, &isr03);
    idt_set(4, &isr04);
    idt_set(5, &isr05);
    idt_set(6, &isr06);
    idt_set(7, &isr07);
    idt_set(8, &isr08);
    idt_set(9, &isr09);
    idt_set(10, &isr10);
    idt_set(11, &isr11);
    idt_set(12, &isr12);
    idt_set(13, &isr13);
    idt_set(14, &isr14);
    /* Interrupt vector 15 is Intel reserved. */
    idt_set(16, &isr16);
    idt_set(17, &isr17);
    idt_set(18, &isr18);
    idt_set(19, &isr19);
    idt_set(20, &isr20);
    /* Interrupt vectors 21-31 are Intel reserved. */

    idt_set(32, &isr32);
    idt_set(33, &isr33);
    idt_set(34, &isr34);
    idt_set(35, &isr35);
    idt_set(36, &isr36);
    idt_set(37, &isr37);
    idt_set(38, &isr38);
    idt_set(39, &isr39);
    idt_set(40, &isr40);
    idt_set(41, &isr41);
    idt_set(41, &isr42);
    idt_set(42, &isr43);
    idt_set(43, &isr44);
    idt_set(44, &isr45);
    idt_set(45, &isr46);
    idt_set(46, &isr47);
}

_Noreturn
uint32_t handle_exception(interrupt_frame_t* frame) {
    print("\nCPU Exception: ");
    print(exception_msgs[frame->int_no]);
    halt();
}
