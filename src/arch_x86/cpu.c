/**
 * CPU
 */

#include "arch_x86/cpu.h"

void idle() {
    while(1) {
        asm("hlt");
    }
}

_Noreturn
void halt() {
    asm("cli \n"
        "hlt");
}
