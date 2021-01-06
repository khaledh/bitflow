#include <stdint.h>
#include "../kernel/util.h"
#include "console.h"

#define VIDEO_MEMORY_ADDR 0xB8000
#define SCREEN_ROWS 25
#define SCREEN_COLS 80

#define OFFSET(row, col) (row * SCREEN_COLS + col)
#define COL(offset) (offset % SCREEN_COLS)

uint16_t* const video_memory = (uint16_t* const)VIDEO_MEMORY_ADDR;
uint16_t current_offset = 0;

int put_char_at(char ch, char attr, int offset) {
    int advance = 1;
    if (ch == '\n') {
        return offset + (SCREEN_COLS - COL(offset));
    }
    if (ch == '\r') {
        return offset - COL(offset);
    }
    if (ch == '\b') {
        if (COL(offset) == 0) {
            return offset;
        }
        ch = 0;
        attr = DEFAULT_COLOR;
        offset = offset - 1;
        advance = 0;
    }
    *(video_memory + offset) = (attr << 8) | ch;
    return advance ? offset + 1 : offset;
}

int put_str_at(const char* str, char attr, int offset) {
    for (const char* p = str; *p; p++) {
        offset = put_char_at(*p, attr, offset);
    }
    return offset;
}

#if 0
int debug_line = 6;
void debug_hex32(uint32_t value) {
    char hex[] = "________";
    to_hex32(value, hex);
    put_str_at(hex, WHITE_ON_BLUE, debug_line * SCREEN_COLS);
    debug_line++;
}
#endif

/**
 * Public functions
 */

void clear_screen() {
    for (int i = 0; i < (SCREEN_ROWS * SCREEN_COLS); i++) {
        put_char_at(0, 0, i);
    }
    current_offset = 0;
}

void put_char(char ch, char attr, int row, int col) {
    put_char_at(ch, attr, OFFSET(row, col));
}

void put_str(const char* str, char attr, int row, int col) {
    put_str_at(str, attr, OFFSET(row, col));
}

void write_char(char ch, char attr) {
    current_offset = put_char_at(ch, attr, current_offset);
}

void write_str(const char* str, char attr) {
    current_offset = put_str_at(str, attr, current_offset);
}

void print_char(char ch) {
    write_char(ch, DEFAULT_COLOR);
}

void print(const char* str) {
    write_str(str, DEFAULT_COLOR);
}

void print_hex8(uint8_t value) {
    char hex[] = "__";
    to_hex8(value, hex);
    print(hex);
}

void print_hex16(uint16_t value) {
    char hex[] = "____";
    to_hex16(value, hex);
    print(hex);
}

void print_hex32(uint32_t value) {
    char hex[] = "________";
    to_hex32(value, hex);
    print(hex);
}