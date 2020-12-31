# Reading Keyboard Input

Unfortunately reading keyboard input is not a simple task, requiring us to setup interrupts and install an IRQ handler. Instead of focusing on the complexity at this point, let's enjoy being foolish for a moment and get something quick and dirty done before we tackle interrupts.

The PS/2 keyboard interface provides two ports: a data port `0x60`, and a command/status port `0x64`. When a key is pressed/released, we should be able to read port `0x60` to get the _scancode_ of the key. A scancode is just a unique byte (or sequence of bytes) for each key on the keyboard. Scancodes are not ASCII (or Unicode) codes; they're a standard set of codes that IBM designed for the original PC. For example, the scancode for the key **A** is `0x1E`, and for the **Left Ctrl** key is `0x1D` [^1].

### Reading keyboard scancodes

Before reading the data port we need to make sure the keyboard has a scancode ready to be read (i.e. a key has been pressed or released). When a scancode is available, the keyboard controller will set bit 0 of the status register, which is the "Output buffer status" bit ("output" from the point of view of the keyboard, not the host).

Let's write a loop to watch that bit, and as soon as it is set, we'll read the data port and display the scancode on screen.


```c
// kernel.c

...
#include "port.h"

void kmain() {
    clear_screen();

    print("Booting kernel...\n");

    while ((port_in8(0x64) & 0x1) == 0) {
        asm("pause");
    }

    uint8_t scancode = port_in8(0x60);
    print_hex8(scancode);

    halt();
}
```

The `asm("pause")` instruction is a hint to the processor that we're executing a spin loop, which helps make the loop power-efficient. Let's run it.

```
Booting Kernel...
1e      <--  after the A key is pressed
```

### Reading a string of scancodes

Great, we are able to read a scancode from the keyboard once a key is pressed. Let's modify this a bit to keep reading scan codes until the **Enter** key is pressed.

```c
// kernel.c

...
#include "port.h"

void kmain() {
    clear_screen();

    print("Booting kernel...\n");

    uint8_t scancode = 0;
    while (scancode != 0x1C) { // 0x1C is the 'Enter' key
        while ((port_in8(0x64) & 0x1) == 0) {
            asm("pause");
        }
        scancode = port_in8(0x60);
        print_hex8(scancode);
        print(" ");
    }
    print("\nBye");

    halt();
}
```

Now let's run this and type the word `hello` followed by an `Enter`.

```
Booting Kernel...
23 a3 12 92 26 a6 26 a6 18 98 1c
Bye
```

For each key we press we get two scancodes in a row, one for the key press and another for the key release. Those are called "make" and "break" scancodes, respectively [^2]. In Scan Code Set 1 the break codes are the same as the make codes with bit 7 set (i.e. the most significant bit). So basically we can associate the above scancodes with the key events as follows:

```
23 a3 12 92 26 a6 26 a6 18 98 1c
h↓ h↑ e↓ e↑ l↓ l↑ l↓ h↑ o↓ o↑ Enter↓
```

### Scancode conversion

It's going to be hard to work with raw scancodes, so we need to convert them to ASCII code. Let's create a lookup table to map the scancodes to ASCII.

```c
const char scancode_to_ascii[] = {
    [0x02] = '1',    [0x10] = 'q',    [0x1E] = 'a',    [0x2C] = 'z',
    [0x03] = '2',    [0x11] = 'w',    [0x1F] = 's',    [0x2D] = 'x',
    [0x04] = '3',    [0x12] = 'e',    [0x20] = 'd',    [0x2E] = 'c',
    [0x05] = '4',    [0x13] = 'r',    [0x21] = 'f',    [0x2F] = 'v',
    [0x06] = '5',    [0x14] = 't',    [0x22] = 'g',    [0x30] = 'b',
    [0x07] = '6',    [0x15] = 'y',    [0x23] = 'h',    [0x31] = 'n',
    [0x08] = '7',    [0x16] = 'u',    [0x24] = 'j',    [0x32] = 'm',
    [0x09] = '8',    [0x17] = 'i',    [0x25] = 'k',
    [0x0A] = '9',    [0x18] = 'o',    [0x26] = 'l',
    [0x0B] = '0',    [0x19] = 'p',    [0x1C] = '\n',
};
```

I mapped only the alphanumeric characters and the `Enter` key for now; we'll have a full mapping later. Now let's modify our original code to convert the make scancodes and print the corresponding ASCII character. We'll ignore the break codes for now.

```c
// kernel.c

    ...

    char ch = 0;
    while (ch != '\n') {
        while ((port_in8(0x64) & 0x1) == 0) {
            asm("pause");
        }
        uint8_t scancode = port_in8(0x60);
        // ignore scancodes beyond our lookup table, including break codes
        if (scancode <= 0x32) {
            ch = scancode_to_ascii[scancode];
            print_char(ch);
        }
    }
    print("Bye");

    ...
```

If we run this we should be able to see what we're typing (only alphanumeric characters for now).

```
Booting kernel...
abcdefghijklmnopqrstuvwxyz1234567890
Bye
```

### Keyboard module

We're starting to accumulate keyboard specific code in the kernel main module, so let's move this new logic into its own module. While at it, let's also expose two functions from this module: one for reading a single character, and another for reading an entire line.

```c
// keyboard.h

#pragma once

char read_char();
void read_line(char buf[], size_t size);
```

```c
// keyboard.c

#include <stdint.h>
#include "kbd.h"
#include "port.h"
#include "console.h"

#define KEYBOARD_DATA   0x60
#define KEYBOARD_STATUS 0x64

const char scancode_to_ascii[] = {
    [0x02] = '1',    [0x10] = 'q',    [0x1E] = 'a',    [0x2C] = 'z',
    [0x03] = '2',    [0x11] = 'w',    [0x1F] = 's',    [0x2D] = 'x',
    [0x04] = '3',    [0x12] = 'e',    [0x20] = 'd',    [0x2E] = 'c',
    [0x05] = '4',    [0x13] = 'r',    [0x21] = 'f',    [0x2F] = 'v',
    [0x06] = '5',    [0x14] = 't',    [0x22] = 'g',    [0x30] = 'b',
    [0x07] = '6',    [0x15] = 'y',    [0x23] = 'h',    [0x31] = 'n',
    [0x08] = '7',    [0x16] = 'u',    [0x24] = 'j',    [0x32] = 'm',
    [0x09] = '8',    [0x17] = 'i',    [0x25] = 'k',
    [0x0A] = '9',    [0x18] = 'o',    [0x26] = 'l',
    [0x0B] = '0',    [0x19] = 'p',    [0x1C] = '\n',
};

uint8_t read_scancode() {
    while ((port_in8(KEYBOARD_STATUS) & 0x1) == 0) {
        asm("pause");
    }
    return port_in8(KEYBOARD_DATA);
}

char read_char() {
    uint8_t scancode;
    while ((scancode = read_scancode()) > 0x32);

    char ch = scancode_to_ascii[scancode];
    print_char(ch);
    return ch;
}

char line_buf[41];

void read_line(char buf[], size_t size) {
    int i = 0;
    char ch;
    while (i < (size - 1) && (ch = read_char()) != '\n') {
        buf[i++] = ch;
    }
    buf[i] = 0;
}
```

### Simple shell

Let's make use of our new capability to write a very simple shell. The shell will read the name of a task, and then will call `exec()` to load and execute the task by name. If the given name doesn't match an existing task name, the shell will output the message `"Task not found"`.

```c
// kernel.c

...
#include "keyboard.h"

void shell();

void kmain() {
    clear_screen();
    print("Booting kernel...\n");

    shell();

    print("\nBye");
    halt();
}

void shell() {
    char name[32];

    print("\n> ");
    read_line(name, 32);
    while (strcmp(name, "quit") != 0) {
        if (exec(name) != 0) {
            print("Task not found.");
        }
        print("\n> ");
        read_line(name, 32);
    }
}
```

If we run this shell we should be able to type "task1" or "task2" and have them executed by the shell.

```
Booting kernel...

> task1
Task 1
> task2
Task 2
> foo
Task not found.
> quit

Bye
```

Great!

### Recap

* We were able to read scan codes from the keyboard and convert them to ASCII characters.
* We implemented a very simple shell that reads the name of a task from the keyboard and executes the task if found.

### Next Steps

Polling the keyboard controller for available data is definitely not efficient. A better approach is to use interrupts, i.e. let the keyboard controller tell us when data is available to be read. We're going to setup interrupts and install a keyboard interrupt handler to allow us to do that.


[^1]: These scancodes are from Scan Code Set 1, the original IBM XT scan code set. Scan Code Set 2 was introduced with the IBM AT and is the one used by modern keyboards. However, the keyboard controller translates scan codes to Scan Code Set 1 by default for backward compatibility, unless configured to disable this translation.

[^2]: We didn't get a break code for the `Enter` key because once we receive the make code we exit the loop and halt, ignoring the break code.