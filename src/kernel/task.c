#include "../arch_x86/cpu.h"
#include "../device/console.h"
#include "task.h"

#define push(sp, val) (*--sp = val)

thread_t t0, t1, t2;
thread_t* current_tcb = &t0;
thread_t* thread_list = &t0;

uint32_t stack1[64], stack2[64];

void thread1() {
    for (int row = 1; row <= 10; row++) {
        for (int col = 0; col < 80; col++) {
            put_char('.', (GRAY_DK << 4 | RED_LT), row, col);
            for (int i=0; i<5000; i++);
        }
    }
}

void thread2() {
    for (int row = 11; row <= 20; row++) {
        for (int col = 0; col < 80; col++) {
            put_char('.', (GRAY_DK << 4 | GREEN_LT), row, col);
            for (int i=0; i<10000; i++);
        }
    }
}

void add_task(thread_t* t) {
    thread_t* curr = thread_list;
    thread_t* next = curr->next;
    while (next != thread_list) {
        curr = curr->next;
        next = curr->next;
    }
    curr->next = t;
    t->next = next;
}

void del_task(thread_t* t) {
    thread_t* prev = thread_list;
    thread_t* curr = prev->next;
    while (curr != t && prev->next != thread_list) {
        prev = prev->next;
        curr = prev->next;
    }
    if (prev == t) {
        prev->next = curr->next;
    }
}

void init_task() {
    asm("sti");
}

void fini_task(thread_t* t) {
//    if (current_tcb == t) {
//        current_tcb = current_tcb->next;
//    }
//    del_task(t);
    t->status = 0;
    idle();
}

void create_task(thread_t* t, uint32_t* stack, void (*entry_point)()) {
    // store thread entry point on the stack (it will be popped off during the ret from task_switch)
    push(stack, (uint32_t)fini_task);    // eip
    push(stack, (uint32_t)entry_point);  // eip
    push(stack, (uint32_t)init_task);    // eip
    push(stack, 0);        // eax
    push(stack, 0);        // ecx
    push(stack, 0);        // edx
    push(stack, 0);        // ebx
    push(stack, 0);        // esp
    push(stack, 0);        // ebp
    push(stack, 0);        // esi
    push(stack, 0);        // edi

    t->esp = (uint32_t)stack;
    t->status = 1;

    add_task(t);
}

void tasking_init() {
    t0.status = 1;
    thread_list = &t0;
    thread_list->next = thread_list;

    create_task(&t1, &stack1[64], thread1);
    create_task(&t2, &stack2[64], thread2);
}
