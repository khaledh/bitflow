/**
 * PS/2 Keyboard
 */

#include <stdint.h>
#include "../arch_x86/port.h"
#include "console.h"
#include "kbd.h"

#define KEYBOARD_DATA   0x60
#define KEYBOARD_STATUS 0x64

const char scancode_to_ascii[] = {
    [0x02] = '1',    [0x10] = 'q',    [0x1E] = 'a',    [0x2C] = 'z',
    [0x03] = '2',    [0x11] = 'w',    [0x1F] = 's',    [0x2D] = 'x',
    [0x04] = '3',    [0x12] = 'e',    [0x20] = 'd',    [0x2E] = 'c',
    [0x05] = '4',    [0x13] = 'r',    [0x21] = 'f',    [0x2F] = 'v',
    [0x06] = '5',    [0x14] = 't',    [0x22] = 'g',    [0x30] = 'b',
    [0x07] = '6',    [0x15] = 'y',    [0x23] = 'h',    [0x31] = 'n',
    [0x08] = '7',    [0x16] = 'u',    [0x24] = 'j',    [0x32] = 'm',
    [0x09] = '8',    [0x17] = 'i',    [0x25] = 'k',
    [0x0A] = '9',    [0x18] = 'o',    [0x26] = 'l',
    [0x0B] = '0',    [0x19] = 'p',    [0x1C] = '\n',
};

uint8_t read_scancode() {
    while ((port_in8(KEYBOARD_STATUS) & 0x1) == 0) {
        asm("pause");
    }
    return port_in8(KEYBOARD_DATA);
}

char read_char() {
    uint8_t scancode;
    while ((scancode = read_scancode()) > 0x32);

    char ch = scancode_to_ascii[scancode];
    print_char(ch);
    return ch;
}

void read_line(char buf[], size_t size) {
    int i = 0;
    char ch;
    while (i < (size - 1) && (ch = read_char()) != '\n') {
        buf[i++] = ch;
    }
    buf[i] = 0;
}
