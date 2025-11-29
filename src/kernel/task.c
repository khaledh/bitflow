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

uint32_t n_tasks = 0;

task_t tasks[16];
task_t* current_task = NULL;

uint32_t kstacks[8][STACK_SIZE];
uint32_t ustacks[8][STACK_SIZE];


void add_task(task_t* t) {
//    task_t* curr = thread_list;
//    task_t* next = curr->next;
//    while (next != thread_list) {
//        curr = curr->next;
//        next = curr->next;
//    }
//    curr->next = t;
//    t->next = next;

  //  print("Adding task ");
  //  print_hex8(t->id);
  //  print(": tasks[");
  //  print_hex8(t->id - 1);
  //  print("].next = task ");
  //  print_hex8(t->id);
  //  print("\n");

  if (t->id > 0) {
    tasks[t->id - 1].next = t;
    t->next = &tasks[0];
  } else {
    t->next = t;
  }

//    print("Task 0 next: ");
//    print_hex8(tasks[0].next->id);
//    print("\n");
}

// Remove a task from the circular linked list
static void remove_task(task_t* t) {
    // Find the task that points to t
    task_t* prev = t;
    while (prev->next != t) {
        prev = prev->next;
    }
    // Unlink t from the list
    prev->next = t->next;
    t->next = NULL;
}

void end_task() {
    task_t* t = get_current_task();

    // Mark this task as terminated and remove from scheduler list
    t->state = TERMINATED;
    remove_task(t);

    // Yield the CPU forever; the scheduler (called from timer interrupt)
    // will switch to another task.
    for (;;) {
        asm volatile("sti\nhlt");
    }
}

task_t* create_task(const char* name, void (*entry_point)(int)) {
    int tid = n_tasks++;
    task_t* t = &tasks[tid];
    uint32_t* kstack = &kstacks[tid][STACK_SIZE];

    push(kstack, tid);                   // task id (param to entry_point)
    push(kstack, (uint32_t)end_task);    // eip for ret
    push(kstack, 0x202);                 // eflags
    push(kstack, 0x08);                  // cs
    push(kstack, (uint32_t)entry_point); // eip
    push(kstack, 0);                     // error_code
    push(kstack, 0);                     // int_no
    push(kstack, 0);                     // eax
    push(kstack, 0);                     // ecx
    push(kstack, 0);                     // edx
    push(kstack, 0);                     // ebx
    push(kstack, 0);                     // esp
    push(kstack, 0);                     // ebp
    push(kstack, 0);                     // esi
    push(kstack, 0);                     // edi
    push(kstack, 0x10);                  // ds
    push(kstack, 0x10);                  // es
    push(kstack, 0x10);                  // fs
    push(kstack, 0x10);                  // gs

    t->esp = (uint32_t)kstack;
    t->kstack = (uint32_t)&kstacks[tid][STACK_SIZE];
    t->state = NEW;
    t->id = tid;
    t->privilege = 0;
    t->keybuf = create_blocking_queue();
    strncpy(t->name, name, 32);

  //  print("Created task ");
  //  print_hex8(t->id);
  //  print(" state: ");
  //  print_hex8(t->state);
  //  print("\n");

    add_task(t);

    return t;
}

task_t* create_user_task(const char* name, void (*entry_point)(int)) {
    int tid = n_tasks++;
    task_t* t = &tasks[tid];
    uint32_t* kstack = &kstacks[tid][STACK_SIZE];
    uint32_t* user_esp = &ustacks[tid][STACK_SIZE];

//    push(stack, n_tasks);               // task id (param to entry_point)
//    push(stack, (uint32_t)end_task);    // eip

    push(kstack, 0x20 | 3);              // ss
    push(kstack, (uint32_t)user_esp);    // esp
    push(kstack, 0x202);                 // eflags
    push(kstack, 0x18 | 3);              // cs
    push(kstack, (uint32_t)entry_point); // eip
    push(kstack, 0);                     // error_code
    push(kstack, 0);                     // int_no
    push(kstack, 0);                     // eax
    push(kstack, 0);                     // ecx
    push(kstack, 0);                     // edx
    push(kstack, 0);                     // ebx
    push(kstack, 0);                     // esp
    push(kstack, 0);                     // ebp
    push(kstack, 0);                     // esi
    push(kstack, 0);                     // edi
    push(kstack, 0x20 | 3);              // ds
    push(kstack, 0x20 | 3);              // es
    push(kstack, 0x20 | 3);              // fs
    push(kstack, 0x20 | 3);              // gs

    t->esp = (uint32_t)kstack;
    t->kstack = (uint32_t)&kstacks[tid][STACK_SIZE];
    t->state = NEW;
    t->id = tid;
    t->privilege = 3;
    t->keybuf = create_blocking_queue();
    strncpy(t->name, name, 32);

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

int get_task_list(task_t** task_list) {
    *task_list = tasks;
    return n_tasks;
}
