/**
 * CPU
 */

#include "cpu.h"

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
