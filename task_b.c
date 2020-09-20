#include <stdint.h>
#include "screen.h"

void task_b() {
    *((uint16_t*)0xB8000 + (2 * 80)) = (WHITE_ON_LIGHTBLUE << 8) + 'B';
}
