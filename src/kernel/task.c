/**
 * Task Management
 */

#include "arch_x86/cpu.h"
#include "device/console.h"
#include "kernel/scheduler.h"
#include "kernel/task.h"
#include "lib/queue.h"
#include "lib/util.h"

#define STACK_SIZE 512

//task_t t0, t1, t2;
//task_t* current_task = &t0;
//task_t* thread_list = &t0;

uint32_t n_tasks = 0;

task_t tasks[16];
task_t* current_task = &tasks[0];

uint32_t stacks[8][STACK_SIZE];


void add_task(task_t* t) {
//    task_t* curr = thread_list;
//    task_t* next = curr->next;
//    while (next != thread_list) {
//        curr = curr->next;
//        next = curr->next;
//    }
//    curr->next = t;
//    t->next = next;

    tasks[t->id - 1].next = t;
    tasks[t->id].next = &tasks[0];
}

//void del_task(task_t* t) {
//    task_t* prev = thread_list;
//    task_t* curr = prev->next;
//    while (curr != t && prev->next != thread_list) {
//        prev = prev->next;
//        curr = prev->next;
//    }
//    if (prev == t) {
//        prev->next = curr->next;
//    }
//}

void end_task() {
//    if (current_task == t) {
//        current_task = current_task->next;
//    }
//    del_task(t);

    schedule(TERMINATED);
}

task_t* create_task(void (*entry_point)()) {
    task_t* t = &tasks[n_tasks];
    uint32_t* stack = &stacks[n_tasks - 1][STACK_SIZE];

    push(stack, n_tasks);               // task id (param to entry_point)
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
    t->state = NEW;
    t->id = n_tasks++;
    t->keybuf = create_blocking_queue();

    add_task(t);

    return t;
}

task_t* create_user_task(void (*entry_point)()) {
    task_t* t = &tasks[n_tasks];
    uint32_t* stack = &stacks[n_tasks - 1][STACK_SIZE];

    push(stack, n_tasks);               // task id (param to entry_point)
    push(stack, (uint32_t)end_task);    // eip

    uint32_t* esp = stack;
    push(stack, 0x20 | 3);              // ss
    push(stack, (uint32_t)esp);         // esp
    push(stack, 0x202);                 // eflags
    push(stack, 0x18 | 3);              // cs
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
    push(stack, 0x20 | 3);              // ds
    push(stack, 0x20 | 3);              // es
    push(stack, 0x20 | 3);              // fs
    push(stack, 0x20 | 3);              // gs

    t->esp = (uint32_t)stack;
    t->state = NEW;
    t->id = n_tasks++;
    t->keybuf = create_blocking_queue();

    add_task(t);

    return t;
}

task_t* get_current_task() {
    return current_task;
}

task_t* get_task(uint32_t tid) {
    return &tasks[tid];
}

void set_task_state(uint32_t tid, task_state_t state) {
    tasks[tid].state = state;
}

void tasking_init() {
    tasks[0].id = n_tasks++;
    tasks[0].state = RUNNING;
    tasks[0].next = &tasks[0];
}
