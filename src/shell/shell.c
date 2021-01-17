/**
 * Command Line Shell
 */

#include <stddef.h>
#include "shell.h"
#include "device/console.h"
#include "kernel/loader.h"
#include "lib/util.h"

static int shell_no = 0;
static char colors[] = {
    (BLACK << 4 | YELLOW),
    (BLACK << 4 | RED_LT),
};

char shell_read_char() {
    char ch = bq_dequeue(get_current_task()->keybuf);
    write_char(ch, colors[shell_no]);
    return ch;
}

void shell_read_line(char buf[], size_t size) {
    int i = 0;
    char ch;
    while (i < (size - 1) && (ch = shell_read_char()) != '\n') {
        if (ch == '\b') {
            i--;
        } else {
            buf[i++] = ch;
        }
    }
    buf[i] = 0;
}


void shell(uint32_t task_id) {
    char name[32];

    print("\n> ");
    shell_read_line(name, 32);
    while (strcmp(name, "quit") != 0) {
        int len = strlen(name);
        if (len > 0) {
            if (strcmp(name, "switch") == 0) {
                uint32_t tid = shell_no == 0 ? task_id + 1 : task_id - 1;
                shell_no = (shell_no + 1) % 2;
                set_active_task(get_task(tid));
            }
            else if (exec(name) != 0) {
                print("Task not found.");
            }
            print("\n");
        }
        print("> ");
        shell_read_line(name, 32);
    }

    print("\nBye");
}
