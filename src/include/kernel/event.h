#pragma once

typedef struct event {
    int state;
    int tid;
} event_t;

event_t* create_event();
void set_event(event_t* evt);
void reset_event(event_t* evt);
void wait_event(event_t* evt);
