/**
 * Kernel Utilities
 */

#include <stdint.h>
#include <stddef.h>

const char hex_digits[] = "0123456789abcdef";

void to_hex8(uint8_t value, char* buf) {
    for (int i = 0; i < 2; i++) {
        buf[1 - i] = hex_digits[(value >> (i * 4)) & 0x000F];
    }
    buf[2] = 0;
}

void to_hex16(uint16_t value, char* buf) {
    for (int i = 0; i < 4; i++) {
        buf[3 - i] = hex_digits[(value >> (i * 4)) & 0x000F];
    }
    buf[4] = 0;
}

void to_hex32(uint32_t value, char* buf) {
    for (int i = 0; i < 8; i++) {
        buf[7 - i] = hex_digits[(value >> (i * 4)) & 0xF];
    }
    buf[8] = 0;
}

int strcmp(const char* str1, const char* str2) {
    for(; *str1 && *str2 && *str1 == *str2; str1++, str2++);
    return *str1 - *str2;
}

int strlen(const char* str) {
    const char* p = str;
    while (*p++);
    return p - str - 1;
}

void strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    if (i < n) {
        dest[i] = '\0';
    }
}