#include "kvector.h"
#include "console.h"

kernel_vector_t kernel_vectors[] = {
    [KVECT_PUT_STR] = (kernel_vector_t)put_str
};
