//
// Created by Khaled on 2021-01-17.
//

#include <stdint.h>
#include "arch_x86/port.h"
#include "device/bga.h"


#define VBE_DISPI_BANK_ADDRESS           0xA0000
#define VBE_DISPI_BANK_SIZE_KB           64

#define VBE_DISPI_MAX_XRES               2560
#define VBE_DISPI_MAX_YRES               1600
#define VBE_DISPI_MAX_BPP                32

#define VBE_DISPI_IOPORT_INDEX           0x01CE
#define VBE_DISPI_IOPORT_DATA            0x01CF

#define VBE_DISPI_INDEX_ID               0x0
#define VBE_DISPI_INDEX_XRES             0x1
#define VBE_DISPI_INDEX_YRES             0x2
#define VBE_DISPI_INDEX_BPP              0x3
#define VBE_DISPI_INDEX_ENABLE           0x4
#define VBE_DISPI_INDEX_BANK             0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH       0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT      0x7
#define VBE_DISPI_INDEX_X_OFFSET         0x8
#define VBE_DISPI_INDEX_Y_OFFSET         0x9
#define VBE_DISPI_INDEX_VIDEO_MEMORY_64K 0xa
#define VBE_DISPI_INDEX_DDC              0xb

#define VBE_DISPI_ID0                    0xB0C0
#define VBE_DISPI_ID1                    0xB0C1
#define VBE_DISPI_ID2                    0xB0C2
#define VBE_DISPI_ID3                    0xB0C3
#define VBE_DISPI_ID4                    0xB0C4
#define VBE_DISPI_ID5                    0xB0C5

#define VBE_DISPI_BPP_4                  0x04
#define VBE_DISPI_BPP_8                  0x08
#define VBE_DISPI_BPP_15                 0x0F
#define VBE_DISPI_BPP_16                 0x10
#define VBE_DISPI_BPP_24                 0x18
#define VBE_DISPI_BPP_32                 0x20

#define VBE_DISPI_DISABLED               0x00
#define VBE_DISPI_ENABLED                0x01
#define VBE_DISPI_GETCAPS                0x02
#define VBE_DISPI_8BIT_DAC               0x20
#define VBE_DISPI_LFB_ENABLED            0x40
#define VBE_DISPI_NOCLEARMEM             0x80

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS   0xFD000000

#define VBE_DISPI_TOTAL_VIDEO_MEMORY_KB  (VBE_DISPI_TOTAL_VIDEO_MEMORY_MB * 1024)
#define VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES (VBE_DISPI_TOTAL_VIDEO_MEMORY_KB * 1024)


void write_register(uint16_t index, uint16_t value) {
    port_out16(VBE_DISPI_IOPORT_INDEX, index);
    port_out16(VBE_DISPI_IOPORT_DATA, value);
}

void set_video_mode(uint16_t width, uint16_t height, uint16_t bpp) {
    write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    write_register(VBE_DISPI_INDEX_XRES, width);
    write_register(VBE_DISPI_INDEX_YRES, height);
    write_register(VBE_DISPI_INDEX_BPP, bpp);
    write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768


void fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t colour) {
    uint32_t* fb = (uint32_t*)VBE_DISPI_LFB_PHYSICAL_ADDRESS;

    for (int j = y; j < (y + height); j++) {
        for (int i = x; i < (x + width); i++) {
            *(fb + SCREEN_WIDTH * j + i) = colour;
        }
    }
}

void bga_init() {
    set_video_mode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);

    fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x002B508C);

}
