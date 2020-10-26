#pragma once

#include <stdint.h>
#include "interrupt.h"

void idt_init();
void idt_set(uint8_t vector, interrupt_handler_t handler);
