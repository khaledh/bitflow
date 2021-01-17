#include "device/pic.h"
#include "kernel/exceptions.h"


void isr_handler(interrupt_frame_t frame) {
    (frame.int_no < 32)
        ? handle_exception(&frame)
        : handle_irq(&frame);
}
