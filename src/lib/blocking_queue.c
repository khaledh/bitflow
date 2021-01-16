#include "../kernel/event.h"
#include "queue.h"
#include "blocking_queue.h"

#define next(pos) ((pos + 1) % MAX_ITEMS)

static blocking_queue_t queues[16];
static int n_queues = 0;

blocking_queue_t* create_blocking_queue() {
    if (n_queues >= 16) {
        return 0;
    }
    queues[n_queues].inner = create_queue();
    queues[n_queues].not_empty_evt = create_event();
    return &queues[n_queues++];
}

void bq_enqueue(blocking_queue_t* q, char ch) {
    enqueue(q->inner, ch);
    set_event(q->not_empty_evt);
}

char bq_dequeue(blocking_queue_t* q) {
    wait_event(q->not_empty_evt);
    char ch = dequeue(q->inner);
    if (bq_is_empty(q)) {
        reset_event(q->not_empty_evt);
    }
    return ch;
}

_Bool bq_is_empty(blocking_queue_t* q) {
    return is_empty(q->inner);
}
