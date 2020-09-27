#pragma once

#include <stdint.h>

void to_hex8(uint8_t value, char* buf);
void to_hex16(uint16_t value, char* buf);
void to_hex32(uint32_t value, char* buf);