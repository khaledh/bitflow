/**
 * CPU Interrupts
 */

#pragma once

#include <stdint.h>

typedef void (isr_t)();

typedef struct interrupt_frame {
     uint32_t gs, fs, es, ds;
     uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
     uint32_t int_no;
     uint32_t error_code;
    uint32_t eip, cs, eflags;
} interrupt_frame_t;

typedef void (*interrupt_handler_t)(interrupt_frame_t*);
