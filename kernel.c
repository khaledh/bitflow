#include "kernel.h"
#include "console.h"
#include "loader.h"
#include "cpu.h"
#include "idt.h"
#include "irq.h"
#include "keyboard.h"
#include "timer.h"
#include "task.h"
#include "shell.h"


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
