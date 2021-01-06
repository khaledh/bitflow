#pragma once

typedef struct thread {
    uint32_t esp;
    uint32_t status;
    struct thread* next;
} thread_t;

void tasking_init();
void schedule();
void create_task();
