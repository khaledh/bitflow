/**
 * Sample Task (B)
 */

#include "device/console.h"
#include "kernel/vector.h"

typedef void (*put_str_t)(const char*, char, int, int);

void entry(kernel_vector_t kvectors[]) {
    put_str_t kput_str = (put_str_t)kvectors[VEC_PUT_STR];
    kput_str("Task B", BROWN_ON_GRAY_LT, 1, 73);
}
