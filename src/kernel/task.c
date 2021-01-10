/**
 * Task Management
 */

#include "../arch_x86/cpu.h"
#include "../device/console.h"
#include "task.h"
#include "util.h"

#define STACK_SIZE 512

//thread_t t0, t1, t2;
//thread_t* current_tcb = &t0;
//thread_t* thread_list = &t0;

uint32_t n_tasks = 0;

thread_t threads[16];
thread_t* current_tcb = &threads[0];

uint32_t stacks[8][STACK_SIZE];


void thread(int tid) {
    for (int row = (tid * 3 - 3); row < (tid * 3 - 1); row++) {
        for (int col = 0; col < 80; col++) {
            put_char('.', (BLACK << 4 | tid), row, col);
            for (int i=0; i<250000; i++);
        }
    }
}

void add_task(thread_t* t) {
//    thread_t* curr = thread_list;
//    thread_t* next = curr->next;
//    while (next != thread_list) {
//        curr = curr->next;
//        next = curr->next;
//    }
//    curr->next = t;
//    t->next = next;

    threads[t->id - 1].next = t;
    threads[t->id].next = &threads[0];
}

//void del_task(thread_t* t) {
//    thread_t* prev = thread_list;
//    thread_t* curr = prev->next;
//    while (curr != t && prev->next != thread_list) {
//        prev = prev->next;
//        curr = prev->next;
//    }
//    if (prev == t) {
//        prev->next = curr->next;
//    }
//}

void end_task() {
//    if (current_tcb == t) {
//        current_tcb = current_tcb->next;
//    }
//    del_task(t);
    current_tcb->status = 0;
    idle();
}

void create_task(void (*entry_point)()) {
    thread_t* t = &threads[n_tasks];
    uint32_t* stack = &stacks[n_tasks - 1][STACK_SIZE];

    push(stack, n_tasks);           // task id
    push(stack, (uint32_t)end_task);    // eip
    push(stack, 0x202);                 // eflags
    push(stack, 0x08);                  // cs
    push(stack, (uint32_t)entry_point); // eip
    push(stack, 0);                     // error_code
    push(stack, 0);                     // int_no
    push(stack, 0);                     // eax
    push(stack, 0);                     // ecx
    push(stack, 0);                     // edx
    push(stack, 0);                     // ebx
    push(stack, 0);                     // esp
    push(stack, 0);                     // ebp
    push(stack, 0);                     // esi
    push(stack, 0);                     // edi
    push(stack, 0x10);                  // ds
    push(stack, 0x10);                  // es
    push(stack, 0x10);                  // fs
    push(stack, 0x10);                  // gs

    t->esp = (uint32_t)stack;
    t->status = 1;
    t->id = n_tasks++;

    add_task(t);
}

void tasking_init() {
    threads[0].id = n_tasks++;
    threads[0].status = 1;
    threads[0].next = &threads[0];

    create_task(thread);
    create_task(thread);
    create_task(thread);
    create_task(thread);
    create_task(thread);
    create_task(thread);
    create_task(thread);
    create_task(thread);
}
