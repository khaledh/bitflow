#include "../device/pic.h"
#include "exceptions.h"


uint32_t isr_handler(interrupt_frame_t frame) {
    uint32_t esp = (frame.int_no < 32)
        ? handle_exception(&frame)
        : handle_irq(&frame);

    return (esp == 0) ? frame.esp : esp;
}
