#include <stdint.h>
#include "screen.h"

#define VIDEO_MEMORY_ADDR 0xB8000
#define SCREEN_ROWS 25
#define SCREEN_COLS 80

uint16_t* const video_memory = (uint16_t* const)VIDEO_MEMORY_ADDR;

void clear_screen() {
    for (int i = 0; i < (SCREEN_ROWS * SCREEN_COLS); i++) {
        *(video_memory + i) = 0;
    }
}

void put_char(char ch, char attr, int row, int col) {
    int offset = row * SCREEN_COLS + col;
    *(video_memory + offset) = (attr << 8) + ch;
}
