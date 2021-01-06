#include "shell.h"
#include "console.h"
#include "kbd.h"
#include "util.h"
#include "loader.h"

void shell() {
    char name[32];

    print("\n> ");
    read_line(name, 32);
    while (strcmp(name, "quit") != 0) {
        if (exec(name) != 0) {
            print("Task not found.");
        }
        print("\n> ");
        read_line(name, 32);
    }

    print("\nBye");
}
