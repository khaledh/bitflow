#include "queue.h"
#include "blocking_queue.h"

#define next(pos) ((pos + 1) % MAX_ITEMS)

void enqueue(blocking_queue_t q, char ch) {
    enqueue(q.inner, ch);
}

char dequeue(blocking_queue_t q) {
    if (is_empty(q)) {
        q.empty_callback(q);
    }
}

bool is_empty(blocking_queue_t q) {
    return is_empty(q.inner);
}
