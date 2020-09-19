#include <stddef.h>
#include <stdint.h>

#define TASK_SIZE 512

typedef uint8_t TASK_BLOCK[TASK_SIZE];

extern TASK_BLOCK __tasks_start;
TASK_BLOCK* ptasks = &__tasks_start;

void load_task(int task_index, uint8_t* dst) {
    // uint8_t* src = (uint8_t*)(ptasks + task_index);
    uint8_t* src = (uint8_t*)ptasks;
    for (int i = 0; i < TASK_SIZE; i++) {
        dst[i] = src[i];
    }
}
