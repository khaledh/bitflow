#pragma once

#include <stdint.h>
#include "../kernel/interrupt.h"

void idt_init();
void idt_set(uint8_t vector, void (*handler)());
