#include "event.h"
#include "task.h"
#include "scheduler.h"


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
    if (evt->state == 1) {
        return;
    }
    evt->tid = get_current_task()->id;
    schedule(BLOCKED);
}
