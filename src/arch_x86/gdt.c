/**
 * Global Descriptor Table (GDT)
 */

#include <stdint.h>
#include <arch_x86/tss.h>
#include "arch_x86/gdt.h"

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
        uint16_t code_or_data     :  1;
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
        uint32_t dpadding3        : 21;
    };
    struct {
        uint32_t tpadding1        : 32;
        uint32_t tpadding2        :  8;
        uint32_t sys_type         :  4;
        uint32_t tpadding4        : 20;
    };
    struct {
        uint64_t all;
    };
} seg_desc_t;

static seg_desc_t gdt[16];
static tss_desc_t tss;
static uint8_t kstack[1024];

static struct __attribute__((packed)) {
    uint16_t limit;
    void*    base;
} gdt_desc;

void set_code_seg(seg_desc_t* code_seg, uint32_t base_addr, uint32_t limit, uint16_t dpl) {
    code_seg->base_addr_lo16 = base_addr & 0xffff;
    code_seg->base_addr_md8 = (base_addr >> 0x10) & 0xff;
    code_seg->base_addr_hi8 = (base_addr >> 0x18) & 0xff;
    code_seg->limit_lo16 = limit & 0xffff;
    code_seg->limit_hi4 = (limit >> 0x10) & 0xf;
    code_seg->code_or_data = 1;
    code_seg->code_seg = 1;
    code_seg->code_readable = 1;
    code_seg->dpl = dpl;
    code_seg->seg_32b = 1;
    code_seg->granularity = 1;
    code_seg->present = 1;
}

void set_data_seg(seg_desc_t* data_seg, uint32_t base_addr, uint32_t limit, uint16_t dpl) {
    data_seg->base_addr_lo16 = base_addr & 0xffff;
    data_seg->base_addr_md8 = (base_addr >> 0x10) & 0xff;
    data_seg->base_addr_hi8 = (base_addr >> 0x18) & 0xff;
    data_seg->limit_lo16 = limit & 0xffff;
    data_seg->limit_hi4 = (limit >> 0x10) & 0xf;
    data_seg->code_or_data = 1;
    data_seg->code_seg = 0;
    data_seg->data_writable = 1;
    data_seg->seg_32b = 1;
    data_seg->dpl = dpl;
    data_seg->granularity = 1;
    data_seg->present = 1;
}

void set_tss_seg(seg_desc_t* tss_seg, uint32_t base_addr, uint32_t limit, uint16_t dpl) {
    tss_seg->base_addr_lo16 = base_addr & 0xffff;
    tss_seg->base_addr_md8 = (base_addr >> 0x10) & 0xff;
    tss_seg->base_addr_hi8 = (base_addr >> 0x18) & 0xff;
    tss_seg->limit_lo16 = limit & 0xffff;
    tss_seg->limit_hi4 = (limit >> 0x10) & 0xf;
    tss_seg->code_or_data = 0;  // system-segment descriptor
    tss_seg->sys_type = 0b1001; // 32-bit TSS (available)
    tss_seg->dpl = dpl;
    tss_seg->present = 1;
}

void gdt_init() {
    // set kernel code/data segments
    set_code_seg(&gdt[1], 0x0, 0xfffff, 0);
    set_data_seg(&gdt[2], 0x0, 0xfffff, 0);

    // set user code/data segments
    set_code_seg(&gdt[3], 0x0, 0xfffff, 3);
    set_data_seg(&gdt[4], 0x0, 0xfffff, 3);

    // set task state segment (TSS)
    tss.ss0 = 0x10;
    tss.esp0 = (uint32_t)(kstack + sizeof(kstack));
    set_tss_seg(&gdt[5], (uint32_t)&tss, sizeof(tss) - 1, 3);

    gdt_desc.limit = (sizeof(seg_desc_t) * 6) - 1;
    gdt_desc.base = gdt;

    // load GDT descriptor into CPU
    asm("lgdt %0" : : "m"(gdt_desc));

    // load TSS selector into Task Register
    asm("mov ax, (5 * 8) | 0 \n" // TSS selector with RPL 0
        "ltr ax");
}

void tss_set_kernel_stack(uint32_t esp0) {
    tss.esp0 = esp0;
}
