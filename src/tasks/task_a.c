/**
 * Sample Task (A)
 */

#include "../device/console.h"
#include "../kernel/kvector.h"

typedef void (*put_str_t)(const char*, char, int, int);

void entry(kernel_vector_t kvectors[]) {
    put_str_t kput_str = (put_str_t)kvectors[KVECT_PUT_STR];
    kput_str("Task A", WHITE_ON_BLUE, 1, 0);
}
