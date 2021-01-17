/**
 * Task Scheduler
 */

#include "device/console.h"
#include "kernel/scheduler.h"

extern task_t* current_task;
extern void switch_to_task(task_t*);
extern void switch_to_new_task(task_t*);

void schedule(task_state_t state) {
    current_task->state = state;

//    print("\nswitching from: ");
//    print_hex8(current_task->id);
//    print_hex8(current_task->state);

    task_t* next_task = current_task;
    do {
        next_task = next_task->next;
    } while (next_task->state != READY && next_task->state != NEW);

//    print("\nswitching to: ");
//    print_hex8(next_task->id);
//    print_hex8(next_task->state);

    if (next_task->state == NEW) {
//        print("\nswitching to new task");
        next_task->state = RUNNING;
        switch_to_new_task(next_task);
    } else {
//        print("\nswitching to existing task");
        next_task->state = RUNNING;
        switch_to_task(next_task);
    }
}
