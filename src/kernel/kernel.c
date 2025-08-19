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

_Noreturn void thread(int tid);
_Noreturn void thread2();
_Noreturn void thread3();

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

    create_user_task(thread);
    create_user_task(thread2);
    create_user_task(thread3);
//    create_user_task(thread);
//    for (int i=0; i<250000000; i++);
//    create_user_task(thread);
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

_Noreturn
void thread(int _tid) {
    int tid = 1;
    for (int row = (tid * 3); row < (tid * 3 + 3); row++) {
        for (int col = 0; col < 80; col++) {
//            put_char('.', (BLACK << 4 | tid + 1), row, col);
            put_char('.', (BLACK << 4 | YELLOW), row, col);
            for (int i=0; i<250000; i++);
        }
    }
    for(;;);
}

_Noreturn
void thread2() {
    int tid = 2;
    for (int row = (tid * 3); row < (tid * 3 + 3); row++) {
        for (int col = 0; col < 80; col++) {
//            put_char('.', (BLACK << 4 | tid + 1), row, col);
            put_char('.', (BLACK << 4 | RED_LT), row, col);
            for (int i=0; i<250000; i++);
        }
    }
    for(;;);
}

_Noreturn
void thread3() {
    int tid = 3;
    for (int row = (tid * 3); row < (tid * 3 + 3); row++) {
        for (int col = 0; col < 80; col++) {
//            put_char('.', (BLACK << 4 | tid + 1), row, col);
            put_char('.', (BLACK << 4 | CYAN_LT), row, col);
            for (int i=0; i<250000; i++);
        }
    }
    for(;;);
}
