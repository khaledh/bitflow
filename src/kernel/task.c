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
uint32_t n_tasks = 1;

uint32_t stack1[STACK_SIZE];
uint32_t stack2[STACK_SIZE];
uint32_t stack3[STACK_SIZE];
uint32_t stack4[STACK_SIZE];

thread_t threads[16];

thread_t* current_tcb = &threads[0];

void thread1() {
    for (int row = 2; row <= 6; row++) {
        for (int col = 0; col < 80; col++) {
            put_char('.', (BLACK << 4 | RED_LT), row, col);
            for (int i=0; i<500000; i++);
        }
    }
}

void thread2() {
    for (int row = 7; row <= 11; row++) {
        for (int col = 0; col < 80; col++) {
            put_char('.', (BLACK << 4 | GREEN_LT), row, col);
            for (int i=0; i<500000; i++);
        }
    }
}

void thread3() {
    for (int row = 12; row <= 16; row++) {
        for (int col = 0; col < 80; col++) {
            put_char('.', (BLACK << 4 | YELLOW), row, col);
            for (int i=0; i<500000; i++);
        }
    }
}

void thread4() {
    for (int row = 17; row <= 21; row++) {
        for (int col = 0; col < 80; col++) {
            put_char('.', (BLACK << 4 | BLUE_LT), row, col);
            for (int i=0; i<500000; i++);
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

void create_task(thread_t* t, uint32_t* stack, void (*entry_point)()) {
    // store thread entry point on the stack (it will be popped off during the ret from task_switch)

    push(stack, (uint32_t)end_task);   // eip

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
//    t0.status = 1;
//    t0.id = 0;
//    thread_list = &t0;
//    thread_list->next = thread_list;
//
//    create_task(&t1, &stack1[64], thread1);
//    create_task(&t2, &stack2[64], thread2);

    threads[0].status = 1;
    threads[0].id = 0;
    threads[0].next = &threads[0];

    create_task(&threads[1], &stack1[STACK_SIZE], thread1);
    create_task(&threads[2], &stack2[STACK_SIZE], thread2);
    create_task(&threads[3], &stack3[STACK_SIZE], thread3);
    create_task(&threads[4], &stack4[STACK_SIZE], thread4);

    threads[0].next = &threads[1];
    threads[1].next = &threads[2];
    threads[2].next = &threads[3];
    threads[3].next = &threads[4];
    threads[4].next = &threads[0];
}
