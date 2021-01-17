#pragma once

#include <stdint.h>

void port_out8(uint16_t port, uint8_t byte);
void port_out16(uint16_t port, uint16_t word);

uint8_t port_in8(uint16_t port);
uint16_t port_in16(uint16_t port);
