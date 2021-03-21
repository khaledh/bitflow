#include <device/bga.h>
#include "gui/gui.h"
#include <gui/font.h>
#include <device/console.h>

#define COLOR_WHITE      0x00FFFFFF
#define COLOR_GREY_LT    0x00CCCCCC
#define COLOR_GREY       0x00888888
#define COLOR_GREY_DK    0x00333333
#define COLOR_BG_DEFAULT 0x002B508C

void gui_text(const char* str, uint16_t x, uint16_t y, uint32_t colour) {
    char* p = (char*)str;
    for (int i = 0; *p; i++, p++) {
        bga_copy(font_get_glyph(*p), x + (7 * i), y, 7, 14, colour);
    }
}

void gui_init() {
    font_load();

//    uint8_t* glyph = font_get_glyph('h');
//    print("\nglyph pointer: ");
//    print_hex32((uint32_t)glyph);
//    for (int i=0; i<14; i++) {
//        print("\n");
//        print_hex8(glyph[i]);
//    }
//    return;

    bga_set_graphics_mode();

    bga_rect_fill(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BG_DEFAULT);

    int x = 40;
    int y = 40;
    int width = 500;
    int height = 300;
    int title_height = 25;

    bga_rect(x, y, width, height, COLOR_WHITE);
    bga_rect_fill(x + 1, y + 1, width - 2, height - 2, COLOR_GREY_DK);

    bga_rect(x, y, width, title_height, COLOR_WHITE);
    bga_rect_fill(x + 1, y + 1, width - 1, title_height - 1, COLOR_GREY);

    int text_y = y + title_height + 10;
    int line_height = 16;
    gui_text("hello, world!", x + 10, text_y, COLOR_WHITE);
    gui_text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.", x + 10, text_y + (line_height*2), COLOR_WHITE);
    gui_text("0123456789", x + 10, text_y + (line_height*3), COLOR_WHITE);
}
