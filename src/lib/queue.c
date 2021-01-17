#include "lib/queue.h"
#include "device/console.h"

#define next(pos) ((pos + 1) % QUEUE_MAX_ITEMS)

static queue_t queues[16];
static int n_queues = 0;

//void printq(queue_t* q) {
//    print("[f=");
//    print_hex8(q->front);
//    print(" r=");
//    print_hex8(q->rear);
//    print("]");
//}


queue_t* create_queue() {
    if (n_queues >= 16) {
        return 0;
    }
    queues[n_queues].front = 0;
    queues[n_queues].rear = 0;
    return &queues[n_queues++];
}

void enqueue(queue_t* q, char ch) {
    if (is_full(q)) {
        return;
    }
    q->buf[q->rear] = ch;
    q->rear = next(q->rear);
}

char dequeue(queue_t* q) {
    if (is_empty(q)) {
        return 0;
    }
    char ch = q->buf[q->front];
    q->front = next(q->front);
    return ch;
}

_Bool is_empty(queue_t* q) {
    return q->rear == q->front;
}

_Bool is_full(queue_t* q) {
    return next(q->rear) == q->front;
}
