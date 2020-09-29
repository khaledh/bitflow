# Loading Tasks by Name

Let's recap how we managed to load and execute a task:

```c
// kernel.c

    ...

    uint32_t* load_addr = (uint32_t*)0xf000;

    {
        read_sectors(4 /* start */, 1 /* count */, load_addr);
        task_t task = (task_t)load_addr;
        task();
    }

    ...
```

So in order to load and execute a task we need to know its sector number (we are still assuming each task will be no bigger than 512 bytes). If the task sector changes, we'll have to change the code that loads and executes it. We need a higher level method of loading and executing a task by a unique name, regardless of where it is stored on disk.

To fully achieve this we eventually have to create a file system. But as always we'll start simple and evolve from there. As a start let's encapsulate the laod-and-execute code in the original `task.c` module (the one we used to load tasks from a code section). While doing so, we'll change the `exec` function signature to take a task name. In order to find on which sector the task is located, we'll initially hard code the mapping between task names and their corresponding sectors in the module itself.

Let's check our `os.img` size to see how many sectors it contains.

```
$ wc -c os.img
    3584 os.img
```

That's 7 sectors, which means that our two tasks `task_a` and `task_b` occupy sector 5 and 6, respectively (sector numbers are zero-based). Now let's implement the new `task.c` module.

```c
// task.h

#pragma once

void exec(const char* name);
```

```c
// task.c

#include <stddef.h>
#include <stdint.h>
#include "task.h"
#include "ata.h"
#include "kvector.h"
#include "util.h"

#define TASK_LOAD_ADDR 0xf000

extern kernel_vector_t kernel_vectors[];

typedef void (*task_t)(kernel_vector_t[]);

typedef struct {
    char* name;
    uint32_t sector;
} task_map_t;

task_map_t tasks[] = {
    { "task_a", 5 },
    { "task_b", 6 },
    { NULL,     0 }
};

int lookup_task_sector(const char* name) {
    for (task_map_t *t = tasks; t->name != NULL; t++) {
        if (strcmp(t->name, name) == 0) {
            return t->sector;
        }
    }
    return -1;
}

int load_task(const char* name, uint32_t* dest) {
    int read_count = 0;

    uint32_t sector = lookup_task_sector(name);
    if (sector >= 0) {
        read_count = read_sectors(sector, 1, dest);
    }

    return (read_count == 1) ? 0 : -1;
}

void exec(const char* name) {
    if (load_task(name, (uint32_t*)TASK_LOAD_ADDR) == 0) {
        task_t task = (task_t)TASK_LOAD_ADDR;
        task(kernel_vectors);
    }
}
```

We declare a struct to hold an entry that maps a task name to a sector number, and define an array of this struct to map our two tasks. The `lookup_task_sector` function loops through the array to find a task by its name.

Notice that I'm calling `strcmp` to compare the names. This is not the `strcmp` from the standard C library (since we don't have a C library at run-time), but rather one that I implemented myself in a new `util.c` module.

```c
// util.h
#pragma once

int strcmp(const char* str1, const char* str2);
```

```c
// util.c

#include <stdint.h>

int strcmp(const char* str1, const char* str2) {
    for(; *str1 && *str2 && *str1 == *str2; str1++, str2++);
    return *str1 - *str2;
}
```

Now let's modify the main kernel module to execute each task by name.

```c
// kernel.c

...
#include "task.h"

void kmain() {
    clear_screen();

    print("Booting kernel...");

    exec("task_a");
    exec("task_b");

    halt();
}
```

Let's give it a try:

```
Booting kernel...
Task A
Task B
```

Great, it works!

### Recap

We created a simple way to load and execute tasks by name. We still have the mapping between task names and sector numbers hard coded though.

### Next

???