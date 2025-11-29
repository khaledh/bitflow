/**
 * Task Scheduler
 *
 * This scheduler is called from the ISR common stub (isr_common in isr.asm).
 * The ISR stub saves and restores current_task->esp around the call to isr_handler,
 * so all we need to do here is:
 *   1. Pick the next runnable task
 *   2. Update task states
 *   3. Update current_task pointer and TSS.esp0
 *
 * When isr_common resumes, it will load esp from current_task->esp (which may
 * now point to a different task's kernel stack) and execute the iret epilogue.
 */

#include "kernel/scheduler.h"
#include "arch_x86/gdt.h"

extern task_t* current_task;

void schedule(task_state_t state) {
    task_t* old_task  = current_task;
    task_t* next_task = current_task;

    // Pick the next READY/NEW task in the circular list
    do {
        next_task = next_task->next;
    } while (next_task->state != READY && next_task->state != NEW);

    // Nothing to do if we're staying on the same already-running task
    if (old_task == next_task) {
        return;
    }

    // Update old task's state (unless it was never started, which shouldn't happen here)
    old_task->state = state;

    // Switch logical current task and TSS kernel stack
    current_task = next_task;
    tss_set_kernel_stack(next_task->kstack);

    next_task->state = RUNNING;
}
