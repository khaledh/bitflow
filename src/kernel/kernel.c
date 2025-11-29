/**
 * Kernel entry point and initialization
 */

#include "arch_x86/cpu.h"
#include "arch_x86/gdt.h"
#include "arch_x86/idt.h"
#include "arch_x86/task_switch.h"
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

extern task_t* current_task;

_Noreturn void thread(int _tid);
_Noreturn void thread2(int _tid);
_Noreturn void thread3(int _tid);

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

    //  gui_init();

    task_t* idle_task = create_task("idle", idle);
    task_t* shell_task = create_task("shell", shell);
    create_user_task("dots1",thread);
    create_user_task("dots2",thread2);
    create_user_task("dots3",thread3);

    set_active_task(shell_task);

    // Start executing the idle task as task 0; this never returns.
    current_task = idle_task;
    tss_set_kernel_stack(idle_task->kstack);
    current_task->state = RUNNING;
    resume_new_task(current_task);
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
void thread2(int _tid) {
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
void thread3(int _tid) {
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
