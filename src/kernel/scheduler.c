/**
 * Task Scheduler
 */

#include "scheduler.h"
#include "task.h"

extern thread_t* current_tcb;
extern void switch_to_thread(thread_t* next_thread);

void schedule() {
    while (current_tcb->next->status == 0) {
        current_tcb = current_tcb->next;
    }
    switch_to_thread(current_tcb->next);
}
