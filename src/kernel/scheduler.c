/**
 * Task Scheduler
 */

#include "../device/console.h"
#include "scheduler.h"

extern thread_t* current_tcb;

uint32_t schedule(uint32_t esp) {
    current_tcb->esp = esp;

    int i = 0;
    do {
        current_tcb = current_tcb->next;
        i++;
    } while (current_tcb->status == 0);

    return current_tcb->esp;
}
