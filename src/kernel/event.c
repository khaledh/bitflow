#include "kernel/event.h"
#include "kernel/task.h"
#include "kernel/scheduler.h"


static event_t events[16];
static int n_events = 0;


event_t* create_event() {
    events[n_events].state = 0;
    events[n_events].tid = -1;
    return &events[n_events++];
}

void set_event(event_t* evt) {
    evt->state = 1;
    if (evt->tid != -1) {
        set_task_state(evt->tid, READY);
    }
}

void reset_event(event_t* evt) {
    evt->state = 0;
    evt->tid = -1;
}

void wait_event(event_t* evt) {
    // Block the current task until the event is set.
    // We avoid invoking the scheduler directly from kernel mode;
    // instead we mark the task BLOCKED and yield the CPU with HLT,
    // letting the timer interrupt drive scheduling.
    while (evt->state == 0) {
        evt->tid = get_current_task()->id;
        set_task_state(evt->tid, BLOCKED);

        // Enable interrupts and halt until the next interrupt.
        asm volatile("sti\nhlt");
    }
}
