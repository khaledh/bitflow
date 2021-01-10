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
#include "exceptions.h"
#include "task.h"


void kmain() {
    disable_cursor();
    clear_screen();

    print("Booting kernel...\n");

    idt_init();
    exceptions_init();
    pic_init();
    pit_init();
    keyboard_init();
    tasking_init();

//    create_task(shell);

//     exec("task_a");
//     exec("task_b");

    asm("sti");

//    shell();

    idle();
}
