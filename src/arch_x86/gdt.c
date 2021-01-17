/**
 * Global Descriptor Table (GDT)
 */

#include <stdint.h>
#include "gdt.h"

typedef union {
    struct {
        // bytes 0,1
        uint16_t limit_lo16       : 16;

        // bytes 2,3
        uint16_t base_addr_lo16   : 16;

        // byte 4
        uint16_t base_addr_md8    :  8;

        // byte 5
        uint16_t accessed         :  1;
        uint16_t access           :  2;  // code vs data bits defined below
        uint16_t code_seg         :  1;
        uint16_t not_system       :  1;
        uint16_t dpl              :  2;
        uint16_t present          :  1;

        // byte 6
        uint16_t limit_hi4        :  4;
        uint16_t avl              :  1;
        uint16_t code_64b         :  1;
        uint16_t seg_32b          :  1;
        uint16_t granularity      :  1;

        // byte 7
        uint16_t base_addr_hi8    :  8;
    };
    struct {
        uint32_t cpadding1        : 32;
        uint32_t cpadding2        :  9;
        uint32_t code_readable    :  1;
        uint32_t code_conforming  :  1;
        uint32_t cpadding3        : 21;
    };
    struct {
        uint32_t dpadding1        : 32;
        uint32_t dpadding2        :  9;
        uint32_t data_writable    :  1;
        uint32_t data_expand_down :  1;
        uint32_t dpadding3        :  21;
    };
} seg_desc_t;

static seg_desc_t gdt[3];

static struct __attribute__((packed)) {
    uint16_t limit;
    void*    base;
} gdt_desc;

void set_code_seg(seg_desc_t* code_seg, uint32_t base_addr, uint32_t limit) {
    code_seg->base_addr_lo16 = base_addr & 0xffff;
    code_seg->base_addr_md8 = (base_addr >> 0x10) & 0xff;
    code_seg->base_addr_hi8 = (base_addr >> 0x18) & 0xff;
    code_seg->limit_lo16 = limit & 0xffff;
    code_seg->limit_hi4 = (limit >> 0x10) & 0xf;
    code_seg->not_system = 1;
    code_seg->code_seg = 1;
    code_seg->code_readable = 1;
    code_seg->dpl = 0;
    code_seg->seg_32b = 1;
    code_seg->granularity = 1;
    code_seg->present = 1;
}

void set_data_seg(seg_desc_t* data_seg, uint32_t base_addr, uint32_t limit) {
    data_seg->base_addr_lo16 = base_addr & 0xffff;
    data_seg->base_addr_md8 = (base_addr >> 0x10) & 0xff;
    data_seg->base_addr_hi8 = (base_addr >> 0x18) & 0xff;
    data_seg->limit_lo16 = limit & 0xffff;
    data_seg->limit_hi4 = (limit >> 0x10) & 0xf;
    data_seg->not_system = 1;
    data_seg->code_seg = 0;
    data_seg->data_writable = 1;
    data_seg->seg_32b = 1;
    data_seg->dpl = 0;
    data_seg->granularity = 1;
    data_seg->present = 1;
}

void gdt_init() {
    set_code_seg(&gdt[1], 0x0, 0xfffff);
    set_data_seg(&gdt[2], 0x0, 0xfffff);

    gdt_desc.limit = (sizeof(seg_desc_t) * 3) - 1;
    gdt_desc.base = gdt;

    // load GDT descriptor into CPU
    asm("lgdt %0" : : "m"(gdt_desc));
}
