#include <stdint.h>
#include "task.h"
#include "console.h"
#include "cpu.h"

#define TASK_LOAD_ADDR 0xa000
#define TASK_SIZE 512

typedef uint8_t task_block_t[TASK_SIZE];
typedef void (*task_t)(void);

extern task_block_t __tasks_start;
task_block_t* tasks_base = &__tasks_start;

void load_task(int task_index, uint8_t* dst) {
    uint8_t* src = (uint8_t*)(tasks_base + task_index);
    for (int i = 0; i < TASK_SIZE; i++) {
        dst[i] = src[i];
    }
}

void exec(int task_index) {
    task_t task = (task_t)TASK_LOAD_ADDR;

    // load and execute task
    load_task(task_index, (uint8_t*)TASK_LOAD_ADDR);
    task();
}
