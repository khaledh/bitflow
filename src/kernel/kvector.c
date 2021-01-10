/**
 * Kernel Vectors
 */

#include "../device/console.h"
#include "kvector.h"

kernel_vector_t kernel_vectors[] = {
    [KVECT_PUT_STR] = (kernel_vector_t)put_str,
    [KVECT_PRINT]   = (kernel_vector_t)print,
};
