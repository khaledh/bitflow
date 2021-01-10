/**
 * Command Line Shell
 */

#include "shell.h"
#include "../device/console.h"
#include "../device/kbd.h"
#include "../kernel/loader.h"
#include "../kernel/util.h"

void shell() {
    char name[32];

    print("\n> ");
    read_line(name, 32);
    while (strcmp(name, "quit") != 0) {
        int len = strlen(name);
        if (len > 0) {
            if (exec(name) != 0) {
                print("Task not found.");
            }
            print("\n");
        }
        print("> ");
        read_line(name, 32);
    }

    print("\nBye");
}
