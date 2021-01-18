/**
 * CPU
 */

#include "arch_x86/cpu.h"

_Noreturn
void idle() {
    while(1) {
        asm("hlt");
    }
}

_Noreturn
void halt() {
    while(1) {
        asm("cli \n"
            "hlt");
    }
}
