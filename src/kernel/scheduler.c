/**
 * Task Scheduler
 */

#include "../device/console.h"
#include "scheduler.h"

extern task_t* current_task;

uint32_t schedule(uint32_t esp, task_state_t state) {
    current_task->esp = esp;
    current_task->state = state;

    do {
        current_task = current_task->next;
    } while (current_task->state != READY);

//    print("\nswitched to: ");
//    print_hex8(current_task->id);

    current_task->state = RUNNING;
    return current_task->esp;
}
