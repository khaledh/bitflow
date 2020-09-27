#include <stdint.h>
#include "console.h"

void (*kput_str)(char* str, char attr, int row, int col) = (void *)0x7f69;

void task_b() {
    kput_str("Task B", BROWN_ON_GRAY_LT, 2, 0);
}
