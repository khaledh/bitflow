/**
 * Sample Task (A)
 */

#include "device/console.h"
#include "kernel/vector.h"

typedef void (*put_str_t)(const char*, char, int, int);
typedef void (*print_t)(const char*);

void entry(kernel_vector_t kvectors[]) {
    print_t kprint = (print_t)kvectors[VEC_PRINT];
    kprint("Task A");
}
