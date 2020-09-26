#include <stdint.h>

void port_out8(uint16_t port, uint8_t byte) {
    asm("out dx, al"
        : // no output data
        : "Nd"(port), "a"(byte));
}

void port_out16(uint16_t port, uint16_t word) {
    asm("out dx, ax"
        : // no output data
        : "Nd"(port), "a"(word));
}

uint8_t port_in8(uint16_t port) {
    uint8_t byte;
    asm("in ax, dx"
        : "=a"(byte)
        : "Nd"(port));
    return byte;
}

uint16_t port_in16(uint16_t port) {
    uint16_t word;
    asm("inw ax, dx"
        : "=a"(word)
        : "Nd"(port));
    return word;
}
