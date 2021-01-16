#pragma once

#include <stdint.h>
#include "../lib/queue.h"

typedef enum task_state {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} task_state_t;

typedef struct task {
    uint32_t esp;
    uint32_t id;
    task_state_t state;
    queue_t* keybuf;
    struct task* next;
} task_t;

void tasking_init();
task_t* create_task(void (entry_point)());
task_t* get_current_task();
