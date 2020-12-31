#include <stdint.h>
#include "keyboard.h"
#include "interrupt.h"
#include "irq.h"
#include "port.h"
#include "console.h"

#define MAX_ITEMS    65
#define next(pos)    ((pos + 1) % MAX_ITEMS)

static char buf[MAX_ITEMS];
static int front = 0;
static int rear = 0;

static void enqueue(char ch) {
    if (next(rear) == front) {
        return;
    }
    buf[rear] = ch;
    rear = next(rear);
}

static char dequeue() {
    if (rear == front) {
        return 0;
    }
    char ch = buf[front];
    front = next(front);
    return ch;
}

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
uint8_t kbd_us[128] = {
    [0x00] = 0,                     [0x1E] = 'a',                   [0x3C] = 0,    /* F2 */
    [0x01] = 0x1B, /* Esc */        [0x1F] = 's',                   [0x3D] = 0,    /* F3 */
    [0x02] = '1',                   [0x20] = 'd',                   [0x3E] = 0,    /* F4 */
    [0x03] = '2',                   [0x21] = 'f',                   [0x3F] = 0,    /* F5 */
    [0x04] = '3',                   [0x22] = 'g',                   [0x40] = 0,    /* F6 */
    [0x05] = '4',                   [0x23] = 'h',                   [0x41] = 0,    /* F7 */
    [0x06] = '5',                   [0x24] = 'j',                   [0x42] = 0,    /* F8 */
    [0x07] = '6',                   [0x25] = 'k',                   [0x43] = 0,    /* F9 */
    [0x08] = '7',                   [0x26] = 'l',                   [0x44] = 0,    /* F10 */
    [0x09] = '8',                   [0x27] = ';',                   [0x45] = 0,    /* Num Lock */
    [0x0A] = '9',                   [0x28] = '\'',                  [0x46] = 0,    /* Scroll Lock */
    [0x0B] = '0',                   [0x29] = '`',                   [0x47] = '7',  /* Keypad */
    [0x0C] = '-',                   [0x2A] = 0,    /* Shift (L) */  [0x48] = '8',  /* Keypad */
    [0x0D] = '=',                   [0x2B] = '\\',                  [0x49] = '9',  /* Keypad */
    [0x0E] = '\b',                  [0x2C] = 'z',                   [0x4A] = '-',  /* Keypad */
    [0x0F] = '\t',                  [0x2D] = 'x',                   [0x4B] = '4',  /* Keypad */
    [0x10] = 'q',                   [0x2E] = 'c',                   [0x4C] = '5',  /* Keypad */
    [0x11] = 'w',                   [0x2F] = 'v',                   [0x4D] = '6',  /* Keypad */
    [0x12] = 'e',                   [0x30] = 'b',                   [0x4E] = '+',  /* Keypad */
    [0x13] = 'r',                   [0x31] = 'n',                   [0x4F] = '1',  /* Keypad */
    [0x14] = 't',                   [0x32] = 'm',                   [0x50] = '2',  /* Keypad */
    [0x15] = 'y',                   [0x33] = ',',                   [0x51] = '3',  /* Keypad */
    [0x16] = 'u',                   [0x34] = '.',                   [0x52] = '0',  /* Keypad */
    [0x17] = 'i',                   [0x35] = '/',                   [0x53] = '.',  /* Keypad */
    [0x18] = 'o',                   [0x36] = 0,    /* Shift (R) */  [0x54] = 0,
    [0x19] = 'p',                   [0x37] = '*',  /* Keypad */     [0x55] = 0,
    [0x1A] = '[',                   [0x38] = 0,    /* Alt (L) */    [0x56] = 0,
    [0x1B] = ']',                   [0x39] = ' ',                   [0x57] = 0,    /* F11 */
    [0x1C] = '\n',                  [0x3A] = 0,    /* Caps Lock */  [0x58] = 0,    /* F12 */
    [0x1D] = 0,    /* Ctrl (L) */   [0x3B] = 0,    /* F1 */         
};

uint8_t kbd_us_shift[128] = {
    [0x00] = 0,                     [0x1E] = 'A',
    [0x01] = 0,                     [0x1F] = 'S',
    [0x02] = '!',                   [0x20] = 'D',
    [0x03] = '@',                   [0x21] = 'F',
    [0x04] = '#',                   [0x22] = 'G',
    [0x05] = '$',                   [0x23] = 'H',
    [0x06] = '%',                   [0x24] = 'J',
    [0x07] = '^',                   [0x25] = 'K',
    [0x08] = '&',                   [0x26] = 'L',
    [0x09] = '*',                   [0x27] = ':',
    [0x0A] = '(',                   [0x28] = '"',
    [0x0B] = ')',                   [0x29] = '~',
    [0x0C] = '_',                   [0x2A] = 0,
    [0x0D] = '+',                   [0x2B] = '|',
    [0x0E] = 0,                     [0x2C] = 'Z',
    [0x0F] = 0,                     [0x2D] = 'X',
    [0x10] = 'Q',                   [0x2E] = 'C',
    [0x11] = 'W',                   [0x2F] = 'V',
    [0x12] = 'E',                   [0x30] = 'B',
    [0x13] = 'R',                   [0x31] = 'N',
    [0x14] = 'T',                   [0x32] = 'M',
    [0x15] = 'Y',                   [0x33] = '<',
    [0x16] = 'U',                   [0x34] = '>',
    [0x17] = 'I',                   [0x35] = '?',
    [0x18] = 'O',
    [0x19] = 'P',
    [0x1A] = '{',
    [0x1B] = '}',
    [0x1C] = 0,
    [0x1D] = 0,
};

int shift = 0;

__attribute__ ((interrupt))
static void handle_interrupt(interrupt_frame_t* frame) {
    uint8_t scancode = port_in8(0x60);

//    print_hex8(scancode);
//    print(" ");
//    irq_eoi(1);
//    return;

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80) {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
       scancode &= ~0x80;
       if (scancode == 0x2A || scancode == 0x36) {
           shift = 0;
       }
    } else {
       if (scancode == 0x2A || scancode == 0x36) {
           shift = 1;
       }
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
        char ch = shift ? kbd_us_shift[scancode] : kbd_us[scancode];
        enqueue(ch);
        write_char(ch, (GRAY_LT << 4 | RED));
    }

    // send end of interrupt
    irq_eoi(1);
}

/**
 * Install the keyboard IRQ handler at IRQ1.
 */
void keyboard_init() {
    irq_install(1, handle_interrupt);
}
