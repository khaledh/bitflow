# Writing the Kernel in C

So far we've been using assembly, making us able to move seemlessly from the boot loader to the kernel by jumping directly to the kernel entry point absolute address. To start writing the kernel in C, we'll need to find out how to produce the kernel image in a way that still makes it easy to jump to the kernel entry point.

### Prerequisites

Before we begin, it's important to have the right tools at hand first. We need a compiler that targets 32-bit i386 architecture. In terms of the output object code format, we're only interested in flat binary for now (we'll look at ELF later).

Our lives will be much easier if we use a GCC cross-compiler. The default compiler on macOS is Clang/LLVM, which is a fine compiler that can target i386, but the corresponding linker does not produce flat binary output, only Mach-O binaries. I also switch often between macOS and Windows, and a portable solution makes my workflow a lot easier.

So we should get a cross-compiled GCC that targets i386-elf. For Mac, you can get a pre-built toolchair through homebrew: [homebrew-i386-elf-toolchain](https://github.com/nativeos/homebrew-i386-elf-toolchain). On Windows, we can use WSL to build a cross-compiled GCC under Linux. This [tutorial](http://learnitonweb.com/2019/12/09/6-cross-compiler-gcc-on-windows-10-using-wsl-windows-subsystem-for-linux/) should be helpful.

I also like to setup aliases to the common tools (e.g. `gcc`, `ld`, etc) that use the cross-compiled version of GCC while working inside my project directory. This bash snippet should do the trick:

```bash
export PATH=/usr/local/Cellar/i386-elf-gcc/9.2.0/bin:/usr/local/Cellar/i386-elf-binutils/2.31/bin:$PATH

for cmd in gcc ar as ld nm objcopy objdump readelf size strings strip; do
    alias $cmd="i386-elf-$cmd"
done
```

### Compiling a simple C kernel

Let's write a simple C function to act as our kernel entry point and see what it compiles to:

```c
// kernel.c

void kmain() {
    for(;;);
}
```
```
$ gcc -c kernel.c -o kernel.o
$ file kernel.o
kernel.o: ELF 32-bit LSB relocatable, Intel 80386, version 1 (SYSV), not stripped
```

The compiler generated for us a 32-bit ELF object file targeting the i386 architecture. The ELF format has a standard structure with various headers and sections for various purposes (e.g. debugging, relocation, etc). It is meant to be used by an existing operating system that supports ELF for loading and relocating executables. It would be an overkill for us at this point, so we'll use it only as an intermediate step to generate our raw binary kernel.

Let's take a look at the various sections in the object file:

```
$ # -h to show section headers
$ # -w to output in wide format
$ objdump -hw kernel.o

kernel.o:     file format elf32-i386

Sections:
Idx Name          Size      VMA       LMA       File off  Algn  Flags
  0 .text         00000005  00000000  00000000  00000034  2**0  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000000  00000000  00000000  00000039  2**0  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  00000000  00000000  00000039  2**0  ALLOC
  3 .comment      00000012  00000000  00000000  00000039  2**0  CONTENTS, READONLY
  4 .eh_frame     00000034  00000000  00000000  0000004c  2**2  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
```

We get the typical sections such as `.text`, `.data`, and `.bss` among others. We're interested in the `.text` section since it contains the actual code. Notice that its size is 5 bytes, which is a few machine instructions. Let's disassemble that section.

```
# -d to disassemble
# -j to select the section of interest
# -M intel to output assembly instructions in Intel syntax
$ objdump -d -j .text -M intel kernel.o 

kernel.o:     file format elf32-i386


Disassembly of section .text:

00000000 <kmain>:
   0:   55                      push   ebp
   1:   89 e5                   mov    ebp,esp
   3:   eb fe                   jmp    3 <kmain+0x3>
```

The disassembled code is standard C function prologue (building a new stack frame), followed by the infinte loop we coded (note that there's no function epilogue as the compiler detected that this function will never return).

### Generating a raw binary kernel

There are a few ways to achieve this.

#### Linker binary output

We can ask the linker to take our object file and produce a flat binary instead of the default (ELF):

```
$ ld --oformat=binary kernel.o -o kernel.img
i386-elf-ld: warning: cannot find entry symbol _start; defaulting to 0000000008048000
```

While this worked, the linker produced a warning that it cannot find the default entry point symbol called `_start`. This symbol is usually provided by the C runtime, but since we're not relying on a runtime, we can tell the linker to use our kernel `kmain` function symbol as the entry point:

```
$ ld --oformat=binary --entry=kmain kernel.o -o kernel.img
$ file kernel.img
kernel: data
```

This time we don't get any warning. When we inspect the file type we're told it's just "data", i.e. raw binary. Ideally our kernel should contain only the machine instructions corresponding to the `kmain` function. Let's disassemble it and see:

```
$ ndisasm -b 32 kernel.img
00000000  55                push ebp
00000001  89E5              mov ebp,esp
00000003  EBFE              jmp short 0x3
00000005  0000              add [eax],al
00000007  001400            add [eax+eax],dl
0000000A  0000              add [eax],al
[snip]
```

The first three instructions are what we expect. So what's that other code about? It turns out that the linker by default includes all sections marked as `LOAD` in the output. If you look at the list of sections we produced above, you'll see that there are three such sections: `.text`, `.data`, and `.eh_frame`. The `.data` section size is zero (we don't have any global variables declared), so there's nothing to include in the output. The `.eh_frame` section, however is `0x34` bytes in size, so it does get included in the output.

The `.eh_frame` section is used for exception handling (among other uses) at runtime to unwind the stack when an exception is thrown. It is not needed in our case, and we can get rid of it by passing `-fno-asynchronous-unwind-tables` to the compiler. Let's get rid of it and see what we get.

```
$ gcc -fno-asynchronous-unwind-tables -c kernel.c -o kernel.o
$ objdump -hw kernel.o

kernel.o:     file format elf32-i386

Sections:
Idx Name          Size      VMA       LMA       File off  Algn  Flags
  0 .text         00000005  00000000  00000000  00000034  2**0  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000000  00000000  00000000  00000039  2**0  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  00000000  00000000  00000039  2**0  ALLOC
  3 .comment      00000012  00000000  00000000  00000039  2**0  CONTENTS, READONLY

$ ld --oformat=binary --entry=kmain kernel.o -o kernel.img
$ ndisasm -b 32 kernel.img
00000000  55                push ebp
00000001  89E5              mov ebp,esp
00000003  EBFE              jmp short 0x3
```

Great, that's our 5-bytes lean kernel!

#### Compiler options for the linker

We can also pass linker options directly to the compiler, using `-Wl,<option>` without having to go through two steps (notice that we don't pass the `-c` option to let the compiler invoke the linker after compiling):

```
$ gcc -fno-asynchronous-unwind-tables -Wl,--oformat=binary -Wl,--entry=kmain kernel.c -o kernel.o
/usr/local/Cellar/i386-elf-gcc/9.2.0/lib/gcc/i386-elf/9.2.0/../../../../i386-elf/bin/ld: cannot find crt0.o: No such file or directory
/usr/local/Cellar/i386-elf-gcc/9.2.0/lib/gcc/i386-elf/9.2.0/../../../../i386-elf/bin/ld: cannot find -lc
collect2: error: ld returned 1 exit status
```

It looks like the compiler is telling the linker to link the C runtime library, which it can't find because we don't have a cross-compiled C runtime available. That's intentional, since we're writing a new kernel that doesn't have a C runtime yet. So we need to tell the compiler to not include the C runtime using the `-nostdlib` option:

```
$ gcc -nostdlib -fno-asynchronous-unwind-tables -Wl,--oformat=binary -Wl,--entry=kmain kernel.c -o kernel.img
$ ndisasm -b 32 kernel.img
00000000  55                push ebp
00000001  89E5              mov ebp,esp
00000003  EBFE              jmp short 0x3
```

#### objcopy

We can also use the `objcopy` to extract the `.text` section of the object file.

```
$ gcc -c kernel.c -o kernel.o
$ objcopy j .text -O binary kernel.o kernel.img
$ ndisasm -b 32 kernel.img
00000000  55                push ebp
00000001  89E5              mov ebp,esp
00000003  EBFE              jmp short 0x3
```

Notice that in this case we didn't have to use any special compiler options, and we didn't have to use the linker. That's because with `objcopy` we can select the specific sections we want in the output.

### Running the kernel

Let's put together our boot sector and our new 32-bit kernel.

```
$ cat bootsect.img kernel.img > os.img
$ qemu -drive file=os.img,format=raw
```
```
Booting from Hard Disk...
B
```
```
(qemu) xp $eip
0000000000007e03: 0x0000feeb
```

Great! The machine is executing our infinite loop instruction at the expected location.

### Outputting to vide memory

Let's modify our kernel to output the character `K` directly to video memory.

```c
// kernel.c

#define VIDEO_MEMORY       0xB8000
#define LIGHTGRAY_ON_BLACK 0x07

void kmain() {
    *((unsigned short *)VIDEO_MEMORY) = (LIGHTGRAY_ON_BLACK << 8) + 'K';
    for(;;);
}
```

Let's compile and see what the binary looks like[^1].

```
$ gcc -fno-asynchronous-unwind-tables -c kernel.c -o kernel.o
$ ld --oformat=binary --entry=kmain kernel.o -o kernel.img
$ ndisasm -b 32 kernel.img
00000000  55                push ebp
00000001  89E5              mov ebp,esp
00000003  B800800B00        mov eax,0xb8000
00000008  66C7004B07        mov word [eax],0x74b
0000000D  EBFE              jmp short 0xd
```

Looks good. Let's test it.

```
$ cat bootsect.img kernel.img > os.img
$ qemu -drive file=os.img,format=raw
```
```
KeaBIOS (version ...)
Booting from Hard Disk...
B
```

There's our `K` letter at the top-left of the screen.

Let's recap:
* We setup our C development environment using a cross-compiled GCC.
* We compiled a simple C kernel and learned a bit about the ELF format sections.
* We learned how to disassemble a specific ELF section.
* We learned about loadable sections and how to avoid producing unneeded runtime sections.
* We learned multiple ways of producing a flat binary kernel.
* We made the kernel output a character to screen by writing directly to video memory.

Next:
* It's getting tedious typing the same commands over and over. Let's tidy up things a bit by creating a `Makefile` to help streamline our workflow.

[^1]: Don't worry, we'll tidy up things soon by moving those repetitive commands to a `Makefile`.