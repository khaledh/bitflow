/**
 * Command Line Shell
 */

#include <stddef.h>
#include "shell.h"
#include "arch_x86/port.h"
#include "device/console.h"
#include "kernel/loader.h"
#include "kernel/task.h"
#include "lib/util.h"

char shell_read_char() {
    char ch = bq_dequeue(get_current_task()->keybuf);
    print_char(ch);
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

void print_help() {
    print("Available commands:\n");
    print("  help     - Show this help message\n");
    print("  about    - Show system information\n");
    print("  tasks    - List all tasks\n");
    print("  task_a   - Run sample task A\n");
    print("  task_b   - Run sample task B\n");
    print("  quit     - Shutdown the system\n");
}

void print_about() {
    print("Bitflow OS (c) 2020-2025 Khaled Hammouda\n");
    print("Version 1.0\n");
}

void print_task_list() {
    task_t* tasks;
    int n_tasks = get_task_list(&tasks);
    for (int i = 0; i < n_tasks; i++) {
        print_hex8(tasks[i].id);
        print(" ");
        if (tasks[i].privilege == 0) {
            print("(K)");
        } else {
            print("(U)");
        }
        print(" ");
        print(tasks[i].name);
        print(" ");
        switch (tasks[i].state) {
            case NEW:
                print("NEW");
                break;
            case READY:
                print("READY");
                break;
            case RUNNING:
                print("RUNNING");
                break;
            case BLOCKED:
                print("BLOCKED");
                break;
            case TERMINATED:
                print("TERMINATED");
                break;
        }
        print("\n");
    }
}

void dispatch_cmd(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        print_help();
    }
    else if (strcmp(cmd, "about") == 0) {
        print_about();
    }
    else if (strcmp(cmd, "tasks") == 0) {
        print_task_list();
    }
    else {
        if (exec(cmd) != 0) {
            print("Unknown command. Type 'help' for available commands.\n");
        }
    }
}

void shell(int task_id) {
    char cmd[32];

    clear_screen();
    print_about();
    print("\n> ");
    shell_read_line(cmd, 32);
    while (strcmp(cmd, "quit") != 0) {
        if (strlen(cmd) > 0) {
            dispatch_cmd(cmd);
            print("\n");
        }
        print("> ");
        shell_read_line(cmd, 32);
    }

    print("\nBye\n");

    // Shutdown QEMU (ACPI power off)
    port_out16(0x604, 0x2000);
}
