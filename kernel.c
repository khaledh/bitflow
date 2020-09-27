#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "console.h"
#include "task.h"
#include "cpu.h"

void kmain() {
    clear_screen();

    print("Booting kernel...");

    exec("task_a");
    exec("task_b");

    halt();
}
