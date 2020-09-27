#include <stdint.h>
#include "console.h"

void (*kput_str)(const char* str, char attr, int row, int col) = (void *)0x7f91;

void task_b() {
    kput_str("Task B", BROWN_ON_GRAY_LT, 2, 0);
}
