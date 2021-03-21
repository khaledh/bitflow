/**
 * Kernel entry point and initialization
 */

#include "arch_x86/cpu.h"
#include "arch_x86/gdt.h"
#include "arch_x86/idt.h"
#include "device/bga.h"
#include "device/console.h"
#include "device/keyboard.h"
#include "device/pic.h"
#include "device/pit.h"
#include <gui/font.h>
#include <gui/gui.h>
#include "kernel/exceptions.h"
#include "kernel/task.h"
#include "../shell/shell.h"

void thread(int tid);

void kmain() {
    disable_cursor();
    clear_screen();

    print("Booting kernel...\n");

    gdt_init();
    idt_init();
    exceptions_init();
    pic_init();
    pit_init();
    keyboard_init(handle_key_event);
    tasking_init();

//    gui_init();

//    create_task(thread);
//    create_task(thread);
//    create_task(thread);
//    create_task(thread);
//    create_task(thread);
//    create_task(thread);
//    create_task(thread);

    task_t* shell1 = create_task(shell);
//    task_t* shell2 = create_task(shell);
    set_active_task(shell1);

    asm("sti");

    idle();
}

void thread(int tid) {
    for (int row = (tid * 3); row < (tid * 3 + 3); row++) {
        for (int col = 0; col < 80; col++) {
            put_char('.', (BLACK << 4 | tid), row, col);
            for (int i=0; i<250000; i++);
        }
    }
}
