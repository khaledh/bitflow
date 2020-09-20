#include <stdint.h>
#include "screen.h"

#define VIDEO_MEMORY_ADDR 0xB8000
#define SCREEN_ROWS 25
#define SCREEN_COLS 80

#define OFFSET(row, col) (row * SCREEN_COLS + col)

uint16_t* const video_memory = (uint16_t* const)VIDEO_MEMORY_ADDR;
uint8_t current_offset = 0;

int put_char_at(char ch, char attr, int offset) {
    *(video_memory + offset++) = (attr << 8) | ch;
    return offset;
}

int put_str_at(char* str, char attr, int offset) {
    for (char* p = str; *p; p++) {
        offset = put_char_at(*p, attr, offset);
    }
    return offset;
}

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

void put_str(char* str, char attr, int row, int col) {
    put_str_at(str, attr, OFFSET(row, col));
}

void write_char(char ch, char attr) {
    current_offset = put_char_at(ch, attr, current_offset);
}

void write_str(char* str, char attr) {
    current_offset = put_str_at(str, attr, current_offset);
}
