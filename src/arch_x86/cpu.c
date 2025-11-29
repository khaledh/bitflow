/**
 * CPU
 */

#include "arch_x86/cpu.h"

_Noreturn
void idle(int _tid) {
    for(;;){
        asm("hlt");
    }
}

_Noreturn
void halt() {
    for(;;){
        asm("cli \n"
            "hlt");
    }
}
