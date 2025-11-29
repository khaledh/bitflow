#pragma once

#include <stdint.h>

void gdt_init();
void tss_set_kernel_stack(uint32_t esp0);
