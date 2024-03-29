#pragma once

#include <stdint.h>
#include "../kernel/interrupt.h"

void pic_init();

void irq_install(uint8_t irq_no, interrupt_handler_t irq_handler);

void irq_disable_all();
void irq_enable_all();

void irq_disable(uint8_t irq_no);
void irq_enable(uint8_t irq_no);

void handle_irq(interrupt_frame_t* frame);

void irq_eoi(uint8_t irq_no);
