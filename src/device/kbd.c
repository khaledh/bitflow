/**
 * PS/2 Keyboard
 */

#include <stdint.h>
#include "console.h"
#include "kbd.h"
#include "../lib/queue.h"
#include "../kernel/task.h"

char read_char() {
    char ch = bq_dequeue(get_current_task()->keybuf);
    write_char(ch, (BLACK << 4 | YELLOW));
    return ch;
}

void read_line(char buf[], size_t size) {
    int i = 0;
    char ch;
    while (i < (size - 1) && (ch = read_char()) != '\n') {
        if (ch == '\b') {
            i--;
        } else {
            buf[i++] = ch;
        }
    }
    buf[i] = 0;
}
