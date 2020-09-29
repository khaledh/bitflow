# Loading Tasks from Disk &mdash; Part 2

Our goal now is to compile and link tasks separate from the kernel. We'll generate binary from each task and append them to the overall disk image right after the kernel.

In order to do so, we'll need to prepare a separate linker script for task linking. Let's remove that part of the original linker script and move it to a new file.

```
/* kernel.ld */

SECTIONS
{
    .kernel 0x7e00 :
    {
        kernel.o(.text .data .rodata)
        *(.text .data .rodata)
        . = ALIGN(512);
    }
}
```

```
/* task.ld */

SECTIONS
{
    .task :
    {
        *(.text .data .rodata)
        . = ALIGN(512);
    }
}
```

Let's make corresponding changes to the `Makefile` to separate kernel linking from task linking.

```makefile
# Makefile

.DEFAULT_GOAL := all

NASM := nasm
GCC := i386-elf-gcc
LD := i386-elf-ld
QEMU := qemu-system-i386

CFLAGS := -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP
LDFLAGS := --oformat=binary

%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@

##
# boot sector
#
bootsect.bin: bootsect.asm
	$(NASM) $< -o $@

##
# kernel
#
KERNEL_LDFLAGS := $(LDFLAGS) --entry=kmain

KERNEL_SRCS = kernel.c console.c cpu.c task.c port.c ata.c
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o)
KERNEL_DEPS = $(KERNEL_SRCS:.c=.d)

kernel.bin: $(KERNEL_OBJS) kernel.ld
	$(LD) $(KERNEL_LDFLAGS) $(KERNEL_OBJS) -T kernel.ld -o $@

##
# tasks
#
task_a.bin: task_a.o task.ld
	$(LD) $(LDFLAGS) $< -T task.ld -o $@

task_b.bin: task_b.o task.ld
	$(LD) $(LDFLAGS) $< -T task.ld -o $@

##
# OS image

os.img: bootsect.bin kernel.bin task_a.bin task_b.bin
	cat $^ > os.img

##
# other targets
#
.PHONY: all run clean

all: os.img

run: os.img
	$(QEMU) -nic none -drive file=$<,format=raw -monitor stdio

clean:
	$(RM) $(KERNEL_OBJS) $(KERNEL_DEPS) task_*.o task_*.d *.bin os.img

-include $(deps)
```

Let's make sure everything builds properly.

```
$ make
nasm bootsect.asm -o bootsect.bin
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -c kernel.c -o kernel.o
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -c console.c -o console.o
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -c cpu.c -o cpu.o
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -c task.c -o task.o
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -c port.c -o port.o
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -c ata.c -o ata.o
i386-elf-ld --oformat=binary --entry=kmain kernel.o console.o cpu.o task.o port.o ata.o -T kernel.ld -o kernel.bin
i386-elf-ld: task.o:/Users/khaledhammouda/src/osdev/bitflow/task.c:13: undefined reference to `__tasks_start'
make: *** [kernel.bin] Error 1
```

Since we removed the tasks from the kernel linker script, along with `__tasks_start` symbol, the linker is complaining that the symbol is undefined because we still reference it from the `task.c` module that we created to load a task from an section embedded within the kernel. We're going to rewrite this module soon, so let's remove the code there and keep it empty for now. After removing the code in `task.c` let's build again.

```
$ make
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -c task.c -o task.o
i386-elf-ld --oformat=binary --entry=kmain kernel.o console.o cpu.o task.o port.o ata.o -T kernel.ld -o kernel.bin
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -c task_a.c -o task_a.o
i386-elf-ld --oformat=binary task_a.o -T task.ld -o task_a.bin
i386-elf-ld: task_a.o: in function `task_a':
/Users/khaledhammouda/src/osdev/bitflow/task_a.c:5: undefined reference to `kput_str'
make: *** [task_a.bin] Error 1
```

This time the error is in linking `task_a.o`, which still is trying to call the kernel function through the pointer variable `kput_str`, but that symbol cannot be resolved since we've now separated the task from the kernel. So how do get the address of that function? Unfortunately the linker won't be able to help us here, we'll need to find another way.

A simple (but unmaintainable) approach is to find the function pointer address manually (by inspecting the kernel link map) and hard code it in the task. Let's add the `--print-map` flag temporarily to the `KERNEL_LDFLAGS` variable in the makefile, and build the kernel.

```
$ make kernel.bin | grep put_str$
                0x0000000000007f69                put_str
                0x000000000000801c                kput_str
```

The `put_str` symbol is the actual function address, while `kput_str` is a pointer variable that points to the function. Let's hard code the actual function address by assigning it to a function pointer variable within the task itself.

```c
// task_a.c

...

void (*kput_str)(char* str, char attr, int row, int col) = (void *)0x7f69;

void task_a() {
    kput_str("Task A", WHITE_ON_BLUE, 1, 0);
}
```

Let's compile and link the task and take a look at the assembled binary.

```
$ make task_a.bin
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -c task_a.c -o task_a.o
i386-elf-ld --oformat=binary task_a.o -T task.ld -o task_a.bin

$ ndisasm -u task_a.bin | head -20
00000000  55                push ebp
00000001  89E5              mov ebp,esp
00000003  83EC08            sub esp,byte +0x8
00000006  A120000000        mov eax,[0x20]
0000000B  6A00              push byte +0x0
0000000D  6A01              push byte +0x1
0000000F  6A1F              push byte +0x1f
00000011  6824000000        push dword 0x24
00000016  FFD0              call eax
00000018  83C410            add esp,byte +0x10
0000001B  90                nop
0000001C  C9                leave
0000001D  C3                ret
0000001E  6690              xchg ax,ax
00000020  697F0000546173    imul edi,[edi+0x0],dword 0x73615400
00000027  6B2041            imul esp,[eax],byte +0x41
0000002A  006690            add [esi-0x70],ah
0000002D  6690              xchg ax,ax
0000002F  6690              xchg ax,ax
00000031  6690              xchg ax,ax
```

The call to the kernel function is being made at address 16: `call eax`. This is an indirect call through the `eax` register, which was loaded with the instruction at address 6: `mov eax,[0x20]`. So the function pointer variable is stored at address `0x20` (which is part of the `.rodata` section). At address `0x20` the disassembler is treating data as code (it doesn't know it's data), so we should focus our attention on the first 4 bytes at that address, which is the function pointer value: `697F0000` which we need to parse as little endian as `0x00007F69`. This is indeed our hard coded kernel function address.

There's a problem though. We're assuming that the code above starts at address 0, which is incorrect. The kernel loads the task binary at address `0xf000` before executing it. So let's tell `ndisasm` that the origin of the binary is `0xf000` instead of 0.

```
❯ ndisasm -u -o 0xf000 task_a.bin | head -20
0000F000  55                push ebp
0000F001  89E5              mov ebp,esp
0000F003  83EC08            sub esp,byte +0x8
0000F006  A120000000        mov eax,[0x20]
0000F00B  6A00              push byte +0x0
0000F00D  6A01              push byte +0x1
0000F00F  6A1F              push byte +0x1f
0000F011  6824000000        push dword 0x24
0000F016  FFD0              call eax
0000F018  83C410            add esp,byte +0x10
0000F01B  90                nop
0000F01C  C9                leave
0000F01D  C3                ret
0000F01E  6690              xchg ax,ax
0000F020  697F0000546173    imul edi,[edi+0x0],dword 0x73615400
0000F027  6B2041            imul esp,[eax],byte +0x41
0000F02A  006690            add [esi-0x70],ah
0000F02D  6690              xchg ax,ax
0000F02F  6690              xchg ax,ax
0000F031  6690              xchg ax,ax
```

If you look at the `mov` instruction (now at address `0xF006`) that loads the `eax` register with the contents of the function pointer variable, you'll notice that it still thinks that the pointer variable is stored at `0x20`. That's not good, because it is clearly stored at `0xF020`.

You'll notice the same problem with the instruction at address `0xF011: push dword 0x24`. This is the instruction that pushes the address of the string "Task A" on the stack to pass it to the kernel function. The string will be actually stored at `0xF024` when the task is loaded into memory (you can see the ASCII code of the characters - and the terminating zero - starting at that address: `54 61 73 6B 20 41 00`).

If we run this code it won't work. The problem is that the linker by default assumes the code is going to be linked with a starting address of 0.

We can do something similar to what we did for the kernel by telling the linker that the kernel starting address is `0x7e00`. Let's modify the `task.ld` script to tell the linker that the task will be loaded at address `0xf000` in memory.

```
/* task.ld */

SECTIONS
{
    .task 0xf000 :
    {
        *(.text .data .rodata)
        . = ALIGN(512);
    }
}
```

Let's build the task and inspect its binary again.

```
$ make task_a.bin
i386-elf-gcc -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -c task_a.c -o task_a.o
i386-elf-ld --oformat=binary task_a.o -T task.ld -o task_a.bin

$ ndisasm -u -o 0xf000 task_a.bin | head -20
0000F000  55                push ebp
0000F001  89E5              mov ebp,esp
0000F003  83EC08            sub esp,byte +0x8
0000F006  A120F00000        mov eax,[0xf020]
0000F00B  6A00              push byte +0x0
0000F00D  6A01              push byte +0x1
0000F00F  6A1F              push byte +0x1f
0000F011  6824F00000        push dword 0xf024
0000F016  FFD0              call eax
0000F018  83C410            add esp,byte +0x10
0000F01B  90                nop
0000F01C  C9                leave
0000F01D  C3                ret
0000F01E  6690              xchg ax,ax
0000F020  697F0000546173    imul edi,[edi+0x0],dword 0x73615400
0000F027  6B2041            imul esp,[eax],byte +0x41
0000F02A  006690            add [esi-0x70],ah
0000F02D  6690              xchg ax,ax
0000F02F  6690              xchg ax,ax
0000F031  6690              xchg ax,ax
```

The `mov eax,[0xf020]` instruction now refers to the correct memory address of the function pointer at `0xf020`. The `push dword 0xf024` instruction also refers to the correct memory address of the string.

Let's build everything and try it out.

```
Booting kernel...
Task A
Task B
```

It worked! It's definitely not a maintainable setup, but at least proved to ourselves that we can compile/link a task separately from the kernel, and have the kernel load and execute it, and the task is able to call a kernel function at a specific address.

### Recap

* We separated the tasks from the kernel so that we can compile and link them separately.
* We analyzed the task binary and realized that addresses in the binary are relative to 0, not to where the task will be loaded in memory.
* We were able to tell the linker about the load address of the task and got the addresses corrected.
* We were able to load and execute the task, and have it still call the kernel function by hard coding its address in the task.

### Next

Having hard coded function addresses won't get us far, since any change the kernel may cause the function address to change. We will need to improve how a task finds and calls a kernel function.