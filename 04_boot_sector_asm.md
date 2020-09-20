# Boot sector in assembly

We won't be able to go far by manually writing machine code in hex. To make our lives easier we're going to need an assembler. There are many x86 assemblers available, but NASM[^1] seems to be a popular choice, so we'll use that.

[^1]: https://www.nasm.us

So far our tiny boot sector contains just two instructions: `cli` and `hlt`. Let's create a new assembly file to put those instructions in:

```asm
;   bootsect.asm

    cli
    hlt
```

Before we assemble this file we need to pad it with zeros to be 512 bytes in size, and put the boot sector magic number in the last two bytes.

```asm
;   bootsect.asm

    cli
    hlt

    times 508 db 0
    db 0x55, 0xAA
```

Since each of the first two instructions uses one byte, and the magic number is two bytes, that leaves 508 bytes which we set to zero using the `times` prefix.

Let's assemble this file and inspect the output using `xxd`:

```
$ nasm -o bootsect.img bootsect.asm
$ xxd -a bootsect.img
00000000: faf4 0000 0000 0000 0000 0000 0000 0000  ................
00000010: 0000 0000 0000 0000 0000 0000 0000 0000  ................
*
000001f0: 0000 0000 0000 0000 0000 0000 0000 55aa  ..............U.
```

Great! We produced the exact same boot sector  but with the help of an assembler.

There's a small problem though. We hard-coded the number of bytes we need to pad. When we change the file later, we'll have to adjust that number to make sure that the total size of the boot sector is 512 bytes.

To solve this problem, we need to somehow declare the number of zero bytes to be equal to the difference between the location just before the magic number (i.e. 510) and the location just after the last instruction (i.e. after `hlt` in our current file). This would make the number of zeros variable, depending on the location of the last instruction.

The location just before the magic number is easy; it's 510. We can get the location of the instruction NASM is about to produce (i.e. the location after the last instruction) using the symbol `$`. So basically we can rewrite the hard-coded value as:

```
;   bootsect.asm

    cli
    hlt

    times 510-$ db 0
    db 0x55, 0xAA
```

Unfortunately assembling this will produce the following error:

```
$ nasm -o bootsect.img bootsect.asm
bootsect.asm:6: error: non-constant argument supplied to TIMES
```

The reason is that NASM needs to determine the size of all assembled code and data _before_ it can generate the symbol addresses (including the special symbol `$`) the code refers to. When NASM hits the `times` line, it finds the absolute reference `$` and rejects it. If we tell NASM that we're interested in the _relative difference_ between the current position and the beginning of the code, then it would happily calculate that for us. Fortunately, NASM supports the symbol `$$` to refer to the beginning of the current section, so we can just use the expression `$ - $$` to turn the absolute reference into a relative one:

```
;   bootsect.asm

    cli
    hlt

    times 510-($-$$) db 0
    db 0x55, 0xAA
```

This will produce exactly the same thing as the hard-coded version, but is now dynamic based on the contents of the file[^2].

[^2]: Readers familiar with the `org` directive will know that we're not done yet. But I'm leaving this until we need it.

Let's recap:
* We learned how to write a simple assembly program and assemble it using NASM.
* We learned how to reserve space using `db` and the `times` prefix.
* We learned how to make the reserved space dynamic based on the contents of the file.

Next:
* We can't keep relying on the QEMO monitor to check/debug the state of the machine. We'll need some way to display information on screen.