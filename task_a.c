#include <stdint.h>
#include "screen.h"

void task_a() {
    *((uint16_t*)0xB8000 + 80) = (WHITE_ON_LIGHTBLUE << 8) + 'A';
}
