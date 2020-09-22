# Calling Kernel Functions

_Note: We're not going to implement true syscalls yet (i.e. inter-segment/inter-privilege calls)._

The tasks and the kernel are still linked together, but because the compiler generates relative addressing calls, when we load (i.e. copy) the task to a different memory area, the relative addresses of the kernel functions won't be valid anymore.

To see what I mean, let's call `put_str()` from `task_a` and see the generated assembly.

```c
// task_a.c

void task_a() {
    put_str("Task A", WHITE_ON_BLUE, 1, 0);
}
```

```
$ make task_a.o
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -c task_a.c -o task_a.o
$ objdump -S -M intel task_a.o

task_a.o:     file format elf32-i386


Disassembly of section .text:

00000000 <task_a>:
#include <stdint.h>
#include "console.h"

void task_a() {
   0:   55                      push   ebp
   1:   89 e5                   mov    ebp,esp
   3:   83 ec 08                sub    esp,0x8
    put_str("Task A", WHITE_ON_BLUE, 1, 0);
   6:   6a 00                   push   0x0
   8:   6a 01                   push   0x1
   a:   6a 1f                   push   0x1f
   c:   68 00 00 00 00          push   0x0
  11:   e8 fc ff ff ff          call   12 <task_a+0x12>
  16:   83 c4 10                add    esp,0x10
}
  19:   90                      nop
  1a:   c9                      leave  
  1b:   c3                      ret    
```

Take a look at the `call` instruction at line 11. The opcode is `E8`, which is the [opcode](https://c9x.me/x86/html/file_module_x86_id_26.html) for "call near, relative, displacement relative to next instruction". The part of the description about Near calls explains what that means:

> The target operand specifies either an absolute offset in the code segment (that is an offset from the base of the code segment) or a relative offset (a signed displacement relative to the current value of the instruction pointer in the EIP register, which points to the instruction following the CALL instruction).

The generated instruction above is of the second type, i.e. a call to a relative offset. The opcode for the absolute offset is `FF`, which is what we want. But how do we generate a call instruction using an absolute offset to the kernel function?

### Absolute address calls

One way to solve this problem is to use function pointers. By making the call use an indirect operand, i.e. a register or memory address containing the address to call, the compiler should generate an absolute offset call.

Let's declare a function pointer to `put_str` in `console.h` so that we can use it to call the actual kernel function.

```c
// console.h

...

extern void (*kput_str)(char* str, char attr, int row, int col);
```
```c
// console.c

...

void (*kput_str)(char* str, char attr, int row, int col) = put_str;
```

Notice that we had to use a different name for the function pointer (we can't use the same name for both the function itself and the pointer)[^1]. Notice also that we declared the function pointer as `extern` in the header file, and assigned it to the actual function in the implementation file to avoid multiple definitions errors if the header is included by different modules.

[^1]: We could use the same function name if we wrap the function pointer in a struct and export that instead, but that would complicate the programming style too early. We'll revisit this idea when/if we need to.

Let's use the function pointer `kput_str` in `task_a.c`, compile, and inspect the generated instructions.

```c
// task_a.c

void task_a() {
    kput_str("Task A", WHITE_ON_BLUE, 1, 0);
}
```

```
$ make task_a.o
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -c task_a.c -o task_a.o
$ objdump -S -M intel task_a.o

task_a.o:     file format elf32-i386


Disassembly of section .text:

00000000 <task_a>:
#include <stdint.h>
#include "console.h"

void task_a() {
   0:   55                      push   ebp
   1:   89 e5                   mov    ebp,esp
   3:   83 ec 08                sub    esp,0x8
    kput_str("Task A", WHITE_ON_BLUE, 1, 0);
   6:   a1 00 00 00 00          mov    eax,ds:0x0
   b:   6a 00                   push   0x0
   d:   6a 01                   push   0x1
   f:   6a 1f                   push   0x1f
  11:   68 00 00 00 00          push   0x0
  16:   ff d0                   call   eax
  18:   83 c4 10                add    esp,0x10
}
  1b:   90                      nop
  1c:   c9                      leave  
  1d:   c3                      ret    
```

The call generated at line 16 is now using an absolute address (opcode `FF`). The address was stored in the `eax` register at line 6, which shows it's zero because the `kput_str` symbol hasn't been resolved by the linker yet. Let's do a full build and inspect the `kernel.img` output to see if it worked.

```
$ make
...
$ ndisasm -u -o 0x7e00 -s 0x8200 kernel.img
[snip]
00008200  55                push ebp
00008201  89E5              mov ebp,esp
00008203  83EC08            sub esp,byte +0x8
00008206  A1E47F0000        mov eax,[0x7fe4]
0000820B  6A00              push byte +0x0
0000820D  6A01              push byte +0x1
0000820F  6A1F              push byte +0x1f
00008211  681E820000        push dword 0x821e
00008216  FFD0              call eax
00008218  83C410            add esp,byte +0x10
0000821B  90                nop
0000821C  C9                leave
0000821D  C3                ret
[snip]
```

The `task_a.o` module was linked at address `0x8200` (hence the `-s 0x8200` to `ndisasm` to _sync_ interpretation at this point, as disassembly can sometimes go out of as it encounters data intermingled with code). Our call instruction is at `0x8216`, and the address in `eax` was loaded at `0x8206`. This time we can see that the absolute address is not being directly loaded from an immediate value, but from a memory location (since the function pointer is actually a pointer variable) stored at address `0x7fe4`. Let's get the 4-byte value at that location.

```
❯ xxd -s 0x01e4 -l 4 -e kernel.img
000001e4: 00007f44                             D...
```

This time I use `xxd` to extract that 4-byte value, but I needed to convert the memory offset `0x7fe4` to file offset by subtracting the `0x7e00` origin value. The `-l 4` gives the length of the data we want in bytes; the `-e` option tells `xxd` to interpret the value in little-endian (i.e. least significant byte first). We get another memory address `0x7f44`, which should be the address of the `put_str` function. Let's verify that by asking `ld` to print a link map, which shows what address was used for every symbol.

First let's add the `--print-map` flag (temporarily) to `LDFLAGS` in the `Makefile`.

```makefile
LDFLAGS := --oformat=binary --entry=kmain --print-map
```

Then let's run the link step:

```
$ rm kernel.img && make kernel.img | grep put_str$
                0x0000000000007f44                put_str
                0x0000000000007fe4                kput_str
```

There's our two symbols: `kput_str` is the function pointer variable stored at `0x7fe4`, which we inspected earlier; and the `put_str` function symbol with address `0x7f44`, which was the address stored in the function pointer.

Let's modify `task_b.c` to do the same, then build and run to see if everything works.

```c
// task_b.c
...
void task_b() {
    kput_str("Task B", BROWN_ON_GRAY_LT, 2, 0);
}
```

```
$ make run
```
```
Booting kernel...
Task A
Task B
```

Success! We can see the messages from both tasks, in different colors, just as we intended.

### Recap

* We learned about relative address and absolute address calls and their opcodes.
* We used a function pointer to the kernel function we want to call from a task to force an absolute address call.
* We learned how to inspect the generated assembly code and follow the call indirection through the pointer variable.
* We generated a link map to verify the function address is correct.
