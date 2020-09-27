#include <stdint.h>
#include "console.h"

void (*kput_str)(char* str, char attr, int row, int col) = (void *)0x7f69;

void task_a() {
    kput_str("Task A", WHITE_ON_BLUE, 1, 0);
}
