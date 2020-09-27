#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "console.h"
#include "task.h"
#include "cpu.h"

void kmain() {
    clear_screen();

    put_char('K', WHITE_ON_LIGHTBLUE, 0 /* row */, 0 /* col */);

    // load and execute two tasks in sequence
    exec(0);
    exec(1);

    halt();
}
