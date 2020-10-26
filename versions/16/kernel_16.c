#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "console.h"
#include "task.h"
#include "cpu.h"
#include "ata.h"

typedef void (*task_t)(void);

void kmain() {
    clear_screen();

    print("Booting kernel...");

    uint32_t* load_addr = (uint32_t*)0xf000;

    {
        read_sectors(4 /* start */, 1 /* count */, load_addr);
        task_t task = (task_t)load_addr;
        task();
    }

    {
        read_sectors(5 /* start */, 1 /* count */, load_addr);
        task_t task = (task_t)load_addr;
        task();
    }

    halt();
}
