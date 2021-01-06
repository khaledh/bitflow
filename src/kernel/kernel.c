/**
 * Kernel entry point and initialization
 */

#include "../arch_x86/cpu.h"
#include "../arch_x86/idt.h"
#include "../device/console.h"
#include "../device/keyboard.h"
#include "../device/pic.h"
#include "../device/pit.h"
#include "../shell/shell.h"
#include "task.h"


void kmain() {
    clear_screen();

    print("Booting kernel...\n");

    idt_init();
    irq_init();

    timer_init();
    keyboard_init();
    tasking_init();

//    thread_t shell_task;
//    uint32_t shell_stack[128];
//    create_task(&shell_task, &shell_stack, shell);

    asm("sti");

//     exec("task_a");
//     exec("task_b");

//    shell();

    idle();
}
