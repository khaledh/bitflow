/**
 * CPU Exceptions
 */

#include "../arch_x86/idt.h"

__attribute__ ((interrupt))
void divide_error_handler(interrupt_frame_t* frame) {

}

void install_exception_handlers() {
    idt_set(0, divide_error_handler);
}
