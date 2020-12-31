#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "console.h"
#include "task.h"
#include "cpu.h"
#include "idt.h"
#include "irq.h"
#include "keyboard.h"
#include "port.h"
#include "kbd.h"
#include "util.h"

void shell();

void kmain() {
    clear_screen();

    print("Booting kernel...\n");

//    shell();
//
//    print("\nBye");

    // exec("task_a");
    // exec("task_b");

     idt_init();
     irq_init();
     keyboard_init();
     asm("sti");
    
     while(1) {
         asm("hlt");
     }

//    halt();
}

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
}
