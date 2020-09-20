#pragma once

// text mode colors
#define BLACK       0x0
#define BLUE        0x1
#define GREEN       0x2
#define CYAN        0x3
#define RED         0x4
#define MAGENTA     0x5
#define BROWN       0x6
#define GRAY_LT     0x7
#define GRAY_DK     0x8
#define BLUE_LT     0x9
#define GREEN_LT    0xA
#define CYAN_LT     0xB
#define RED_LT      0xC
#define MAGENTA_LT  0xD
#define YELLOW      0xE
#define WHITE       0xF

#define GRAY_LT_ON_BLACK (BLACK << 4 | GRAY_LT)
#define WHITE_ON_BLUE    (BLUE << 4 | WHITE)

void clear_screen();
void put_char(char ch, char attr, int row, int col);
void put_str(char* str, char attr, int row, int col);
void write_char(char ch, char attr);
void write_str(char* str, char attr);