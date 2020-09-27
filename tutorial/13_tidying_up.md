# Tidying up

Our kernel is starting to accumulate various functionality, and some of the code is repeated. Let's take a moment to tidy things up.

### Task Routines

The code for loading and executing a task is repeated twice, so let's move it to a new function called `exec()` that takes the task index to execute. Let's also move all task handling code into a separate module `task.c`.

```c
// task.h

#pragma once

void exec(int task_index);
```

```c
// task.c

#include <stdint.h>
#include "task.h"

#define TASK_LOAD_ADDR 0xa000
#define TASK_SIZE 512

typedef uint8_t task_block_t[TASK_SIZE];
typedef void (*task_t)(void);

extern task_block_t __tasks_start;
task_block_t* tasks_base = &__tasks_start;

void load_task(int task_index, uint8_t* dst) {
    uint8_t* src = (uint8_t*)(tasks_base + task_index);
    for (int i = 0; i < TASK_SIZE; i++) {
        dst[i] = src[i];
    }
}

void exec(int task_index) {
    task_t task = (task_t)TASK_LOAD_ADDR;

    // load and execute task_a
    load_task(task_index, (uint8_t*)TASK_LOAD_ADDR);
    task();
}
```

Then in `kernel.c` we can include `task.h` and call this function twice in a row.

```c
#include "task.h"

void kmain() {
    ...

    // load and execute two tasks in sequence
    exec(0);
    exec(1);
    
    ...
}
```

### Screen Routines

We have code that outputs a character to screen, as well as a function that clears the screen. Let's move that code to a `console.c` module.

```c
// console.h

#pragma once

#define WHITE_ON_LIGHTBLUE 0x9F

void clear_screen();
void put_char(char ch, char attr, int row, int col);
```

```c
// console.c

#include <stdint.h>
#include "console.h"

#define VIDEO_MEMORY_ADDR 0xB8000
#define SCREEN_ROWS 25
#define SCREEN_COLS 80

uint16_t* const video_memory = (uint16_t* const)VIDEO_MEMORY_ADDR;

void clear_screen() {
    for (int i = 0; i < (SCREEN_ROWS * SCREEN_COLS); i++) {
        *(video_memory + i) = 0;
    }
}

void put_char(char ch, char attr, int row, int col) {
    int offset = row * SCREEN_ROWS + col;
    *(video_memory + offset) = (attr << 8) | ch;
}
```

While we can call the `put_char()` function from the kernel, unfortunately we won't be able to call it from the tasks. The reason is that the function call uses relative addressing, but we relocate the task to a different area in memory before executing it, so it will end up calling an invalid address. So for the time being let's just use it in the kernel, and later we'll discuss how to call kernel functions from tasks.

```c
// kernel.c

#include "console.h"

void kmain() {
    clear_screen();

    put_char('K', WHITE_ON_LIGHTBLUE, 0 /* row */, 0 /* col */);

    ...
}
```

### CPU Routines

One last thing we can do is move the code that halts the CPU to a `cpu.c` module.


```c
// cpu.h

#pragma once

void halt();
```

```c
// cpu.c

#include "cpu.h"

void halt() {
    asm("cli \n"
        "hlt");
}
```

### Final Kernel

Let's take a look at our final `kernel.c` after this refactoring.

```c
// kernel.c

#include "kernel.h"
#include "console.h"
#include "task.h"

void kmain() {
    clear_screen();

    put_char('K', WHITE_ON_LIGHTBLUE, 0 /* row */, 0 /* col */);

    // load and execute two tasks in sequence
    exec(0);
    exec(1);

    halt();
}
```

Looks much better!

### Makefile

Let's not forget to update our `Makefile` to include all those new modules.

```makefile
# Makefile

...

OBJECTS := kernel.o cpu.o console.o task.o task_a.o task_b.o
...

%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(OBJECTS) -T linker.ld -o $@
...
```

Since the number of object files is increasing, it would be tedious to add a rule for every object file. Instead, we can use the wildcards `%.o` and `%.c` to make the rule apply to all object files and their inputs.

### Linker Script

Finally, let's adjust the linker script to include the new object files.


```
/* linker.ld */

SECTIONS
{
    .kernel 0x7e00 :
    {
        kernel.o(.text .data .rodata)
        EXCLUDE_FILE (task_*.o) *(.text .data .rodata)
        . = ALIGN(512);
    }

    __tasks_start = .;
    .tasks :
    {
        task_a.o (.text .data .rodata)
        . = ALIGN(512);

        task_b.o (.text .data .rodata)
        . = ALIGN(512);
    }
}
```

We first output the kernel sections, followed by all the other kernel modules sections. Notice that we use the `EXCLUDE_FILE` command to exclude the task object files at this point; otherwise they will end up being output the `.kernel` section.

Let's build and run everything.

```
$ make run
...
```
```
K
A
B
```

### Recap

* We tidied up our code by moving functions to dedicated modules: cpu, screen, and task.
* We linked all kernel modules right after the main kernel module.
* We learned that we cannot call kernel functions directly from loaded tasks.

### Next

We've made it so far without printing much to screen. We're going to take a break from tasks and add a screen routine to display strings and numbers (both in decimal and hex), which should help us put more detailed info on screen about what the kernel is doing.
