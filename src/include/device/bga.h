#pragma once

#include <stdint.h>

//#define SCREEN_WIDTH 1440
//#define SCREEN_HEIGHT 1080
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960

void bga_set_graphics_mode();
void bga_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t colour);
void bga_rect_fill(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t colour);
void bga_copy(uint8_t mask[], uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t colour);
