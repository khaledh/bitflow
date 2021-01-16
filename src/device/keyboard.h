#pragma once

typedef void (*key_event_handler_t)(char);

void keyboard_init(key_event_handler_t key_event_handler);
