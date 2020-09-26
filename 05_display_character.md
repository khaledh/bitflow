# Display a Single Character

Checking the CPU state using QEMU's monitor is useful, but it's going to be tedious if we have to do it every time we need to check a particular state. It would be much better if we can output what we want on the screen to let the code tell us what it is doing.

This is going to be our first interaction with a hardware device, but fortunately we don't have to do everything ourselves (yet). The BIOS provides a set of useful I/O routines for us to use in real-mode. Those routines are accessed through software interrupts.

An interrupt is basically a way to force the CPU to transfer control from whatever it is currently executing to a well-known location to handle the interrupt (and to later return to the original execution flow). It is similar to a function call, but an indirect one. Interrupts in real-mode are numbered from 0 to 255, where each number represents a separate interrupt service routine (ISR). We tell the CPU which interrupt to invoke by issuing the instruction `int n`, where `n` is the interrupt number. Typically ISRs require arguments, which are passed through specific registers (e.g. `eax`).

So, given just a number, how does the CPU know the address of the ISR to call? During the boot process the BIOS stores those addresses in what is called "Interrupt Vector Table" (IVT) in the first 1K of memory (i.e. 0x0000-0x03FF). Each vector (i.e. address) is 4-bytes long (a segment and an offset), and there are 256 entries, hence the 1024-byte IVT size.

Back to our goal: displaying a character on screen. The BIOS video services interrupt is `INT 10h`. This ISR can perform [many functions](https://stanislavs.org/helppc/int_10.html), depending on the the value passed in the `ah` register. The one we're interested in is **INT 10/ah=0E** "Write Text in Teletype Mode":

```
  INT 10h
    AH = 0E
    AL = ASCII character to write
```

Let's add call this interrupt routine at the beginning of our boot sector to display the character `B`:

```asm
;   bootsect.asm

    mov    ah, 0x0E              ; tty mode
    mov    al, 'B'
    int    0x10

    cli
    hlt

    times 510-($-$$) db 0
    db 0x55, 0xAA
```

Let's try this out:

```
$ nasm -o bootsect.bin bootsect.asm
$ qemu -drive file=bootsect.bin,format=raw
```
```
Booting from Hard Disk...
B
```

Success! We might be tempted to continue along this path and write a routine to display a string, including numbers (decimal and hex), but we'll save that until we actually need it.

Let's recap:
* We learned about the BIOS interrupts and the Interrupt Vector Table.
* We learned how to display a character on screen using the BIOS video interrupt `INT 10h`.

Next:
* The boot sector size is very limited; it is meant only as a bootstrapping mechanism to load a more fleshed out boot loader, or load a kernel directly if we don't need a fancy boot loader.