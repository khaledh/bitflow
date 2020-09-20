#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "screen.h"
#include "task.h"
#include "cpu.h"

void kmain() {
    clear_screen();

    write_str("Booting kernel...", GRAY_LT_ON_BLACK);

    // load and execute two tasks in sequence
    exec(0);
    exec(1);

    halt();
}
