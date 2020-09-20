#include "cpu.h"

void halt() {
    asm("cli \n"
        "hlt");
}