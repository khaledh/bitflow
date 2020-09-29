# Loading Tasks from Disk - Part 3

We managed to call the kernel function `put_str` by hard coding its address in the task. This is not going to work once the address of that function changes, which will happen frequently as we change the kernel. We need a better way for the task to find that address at run-time.

### Passing a function pointer to the task

There are a number of ways to do this. What we will do initially is pass the address of the function to the task when the kernel executes it. This way the task will receive a pointer to the function in its arguments that it can use to call the function.

Let's `typedef` the function pointer type and accept an argument of that type in our task.

```c
// task_a.c (and similarily task_b.c)

...

typedef void (*put_str_t)(const char*, char, int, int);

void entry(put_str_t kput_str) {
    kput_str("Task A", WHITE_ON_BLUE, 1, 0);
}
```

Now let's modify our kernel to pass the `put_str` function address to the task when executing it.

```c
// kernel.c

...
#include "console.h"

    ...

    uint32_t* load_addr = (uint32_t*)0xf000;

    {
        read_sectors(4 /* start */, 1 /* count */, load_addr);
        task_t task = (task_t)load_addr;
        task(put_str);
    }

    ...
```

If we build and run we should get the expected output.

### Passing more than one function pointer

This is fine for one function, but what about exposing many functions to the task? We can't just keep adding more arguments. A simple solution is to build an array of function pointers and pass that to the task. In order for the task to know which function pointer is stored at which index of the array, the kernel can define those indices in a header file that the task can import and use to index into the array using the defined constants.

When we build a table of function pointers it's usually called a "vector table"; a vector is just a fancy name for a function pointer. So we'll call this array "kernel vector table", and each element a "kernel vector".

Let's `typedef` the kernel vector type as a function pointer. Let's also define a constant representing our first index into the array which will be the `put_str` vector.

```c
// kvector.h

#pragma once

typedef void (*kernel_vector_t)(void);

#define KVECT_PUT_STR 0
```

Now let's create the actual vector table, which will contain only one vector for now. I'm using the [designated initializer](https://gcc.gnu.org/onlinedocs/gcc/Designated-Inits.html) feature to be explicit about which index maps to which function address.

```c
// kvector.c

#include "kvector.h"
#include "console.h"

kernel_vector_t kernel_vectors[] = {
    [KVECT_PUT_STR] = (kernel_vector_t)put_str
};
```

Let's pass that array to the task.

```c
...
#include "kvector.h"

extern kernel_vector_t kernel_vectors[];

typedef void (*task_t)(kernel_vector_t[]);

    ...

    uint32_t* load_addr = (uint32_t*)0xf000;

    {
        read_sectors(4 /* start */, 1 /* count */, load_addr);
        task_t task = (task_t)load_addr;
        task(kernel_vectors);
    }

    ...
```

Finally, let's modify the task itself to accept this new kernel vector table as an argument.

```c
// task_a.c

...

#include "kvector.h"

typedef void (*put_str_t)(const char*, char, int, int);

void entry(kernel_vector_t kvectors[]) {
    put_str_t kput_str = (put_str_t)kvectors[KVECT_PUT_STR];
    kput_str("Task A", WHITE_ON_BLUE, 1, 0);
}
```

Again, if we build and run we should get the expected output.

### Recap

We were able to overcome the problem of calling a kernel function without hard coding its address in the task by passing a table of kernel function pointers to the task.

### Next

Now that we don't have to worry about fixing hard coded function addresses every time we make a change to the kernel, let's turn our attention to giving tasks unique names, and loading them by name instead of by sector number.
