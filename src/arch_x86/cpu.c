#include "cpu.h"

void idle() {
    while(1) {
        asm("hlt");
    }
}

void halt() {
    asm("cli \n"
        "hlt");
}
