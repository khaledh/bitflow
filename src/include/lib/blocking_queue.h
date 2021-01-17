#pragma once

#include "queue.h"
#include "../kernel/event.h"

typedef struct blocking_queue {
    queue_t* inner;
    event_t* not_empty_evt;
} blocking_queue_t;

blocking_queue_t* create_blocking_queue();
void bq_enqueue(blocking_queue_t* q, char ch);
char bq_dequeue(blocking_queue_t* q);
_Bool bq_is_empty(blocking_queue_t* q);
