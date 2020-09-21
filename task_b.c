#include <stdint.h>
#include "console.h"

typedef void put_str_t(char* str, char attr, int row, int col);

void task_b() {
    put_str_t* func = (put_str_t*)put_str;
    func("Task B", BROWN_ON_GRAY_LT, 2, 0);
}
