#pragma once

#include <stdint.h>

typedef struct thread {
    uint32_t esp;
    uint32_t id;
    uint32_t status;
    struct thread* next;
} thread_t;

void tasking_init();
void create_task();
