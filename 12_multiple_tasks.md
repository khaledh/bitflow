# Multiple Tasks

Note: We're not implementing multitasking yet. This section is about executing different tasks in sequence, not concurrently.

Now that we are able to "load" a simple task and execute it, let's expand on this a bit and allow multiple tasks to be stored, and make the kernel choose which task to execute by its index in the tasks section.

### Add another task

Let's start by adding renaming our existing task from `task.c` to `task_a.c`, and another task `task_b.c`.

```c
// task_a.c
#include "kernel.h"

void task_a() {
    unsigned short* video_memory = (unsigned short*)VIDEO_MEMORY;
    *(video_memory + 80) = (WHITE_ON_LIGHTBLUE << 8) | 'A';
}
```

```c
// task_b.c
#include "kernel.h"


void task_b() {
    unsigned short* video_memory = (unsigned short*)VIDEO_MEMORY;
    *(video_memory + (2 * 80)) = (WHITE_ON_LIGHTBLUE << 8) | 'B';
}
```

Notice that instead of having both tasks output `'T'`, the first task now outputs `'A'` and the second outputs `'B'`, to differentiate between them.

### Link tasks

Let's modify our `Makefile` to accommodate the new task.

```makefile
# Makefile

...

OBJECTS := kernel.o task_a.o task_b.o
...

task_a.o: task_a.c
	$(GCC) $(CFLAGS) -c $< -o $@

task_b.o: task_b.c
	$(GCC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(OBJECTS) -T linker.ld -o $@
...
```

Let's also modify the linker script to include `task_b.o` as a second 512b block.

```
SECTIONS
{
    ...

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

We can now think of the `.tasks` section as an array of task blocks, each of size 512b. Our goal is to allow the kernel to load a task from this array by index.

### Load tasks by index

Let's modify the `load_task()` function to reflect this new requirement.

```c
#define TASK_SIZE 512
typedef uint8_t task_block_t[TASK_SIZE];

extern task_block_t __tasks_start;
task_block_t* tasks_base = &__tasks_start;

void load_task(int task_index, uint8_t* dst) {
    uint8_t* src = (uint8_t*)(tasks_base + task_index);
    for (int i = 0; i < TASK_SIZE; i++) {
        dst[i] = src[i];
    }
}
```

We define a new type called `task_block_t` as an array of 512 bytes to represent a task block. Notice that we changed the section pointer to be of type `task_block_t*` instead of `uint8_t*`. This makes it simpler to add an index to this base pointer to get to the address of a particular task block. We use this feature to assign the address of the desired block to the `src` variable in the function.

### Load and execute tasks in sequence

Now let's modify `kmain()` to use the new task loading function. We'll load and execute the two tasks one after the other.

```c
void load_task(int task_index, uint8_t* dst);

void kmain() {
    ...

    task_t task = (task_t)TASK_LOAD_ADDR;

    // load and execute task_a
    load_task(0, (uint8_t*)TASK_LOAD_ADDR);
    task();

    // load and execute task_b
    load_task(1, (uint8_t*)TASK_LOAD_ADDR);
    task();

    ...
}
```

I'm reusing the same memory area for loading and executing each task in sequence. If all goes well, we should see the letter `'A'` displayed on the second line and the letter `'B'` displayed on the third line.

```
$ make run
...
```
```
K
A
B
```

Awesome!

### Recap

* We added another task and linked it at another 512b block in the `.tasks` section.
* We modified task loading to load a task given its index in the `.tasks` section.
* We modified the kernel to load and execute two tasks in sequence.

### Next

Let's take a moment to tidy everything by moving some functions to separate kernel modules, and keep the kernel `kmain()` simple and readable.
