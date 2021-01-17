/**
 * Kernel Vectors
 */

#include "device/console.h"
#include "kernel/vector.h"

kernel_vector_t kernel_vectors[] = {
    [VEC_PUT_STR] = (kernel_vector_t)put_str,
    [VEC_PRINT]   = (kernel_vector_t)print,
};
