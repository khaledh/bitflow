#include <stdint.h>
#include "console.h"

void (*kput_str)(const char* str, char attr, int row, int col) = (void *)0x7f91;

void task_a() {
    kput_str("Task A", WHITE_ON_BLUE, 1, 0);
}
