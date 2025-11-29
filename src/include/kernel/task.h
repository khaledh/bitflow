#pragma once

#include <stdint.h>
#include "../lib/blocking_queue.h"

typedef enum task_state {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} task_state_t;

typedef struct task {
    uint32_t esp;
    uint32_t kstack;
    uint32_t id;
    uint8_t privilege;
    task_state_t state;
    blocking_queue_t* keybuf;
    struct task* next;
    char name[32];
} task_t;

task_t* create_task(const char* name, void (entry_point)(int));
task_t* create_user_task(const char* name, void (entry_point)(int));
task_t* get_current_task();
task_t* get_task(uint32_t tid);
void set_task_state(uint32_t tid, task_state_t state);
int get_task_list(task_t** task_list);
void end_task(void);