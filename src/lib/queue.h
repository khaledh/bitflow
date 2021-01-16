#pragma once

#define QUEUE_MAX_ITEMS 128

typedef struct queue {
    int front;
    int rear;
    char buf[QUEUE_MAX_ITEMS];
} queue_t;

queue_t* create_queue();
void enqueue(queue_t* q, char ch);
char dequeue(queue_t* q);
_Bool is_empty(queue_t* q);
_Bool is_full(queue_t* q);
