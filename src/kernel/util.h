#pragma once

#include <stdint.h>

#define push(sp, val) (*--sp = val)

void to_hex8(uint8_t value, char* buf);
void to_hex16(uint16_t value, char* buf);
void to_hex32(uint32_t value, char* buf);

int strcmp(const char* str1, const char* str2);
