#pragma once

#include "queue.h"

typedef struct blocking_queue {
    queue_t inner;
    void (*empty_callback)(struct blocking_queue queue);
} blocking_queue_t;

void enqueue(blocking_queue_t q, char ch);
char dequeue(blocking_queue_t q);
bool is_empty(blocking_queue_t q);
