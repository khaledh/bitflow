Now that we understand a bit more about our boot environment, we should take over control from the BIOS once it loads our boot sector at 0x07C00.

Assuming we have a boot sector, how do we tell QEMU to use it as a bootable disk?

Let's check the [QEMU docs](https://www.qemu.org/docs/master/system/index.html). The quick start page mentions that if we have PC hard disk image with Linux installed we can boot it using:

```
$ qemu linux.img
```

So we basically need to create a disk image that QEMU can use. Looking further at the docs we can see that QEMU has a [tool](https://www.qemu.org/docs/master/system/images.html) called `qemu-img` for creating disk images:

```
$ qemu-img create myimage.img mysize
```

OK, let's create a disk image that is one sector in size, i.e. 512 bytes:

```
$ qemu-img create bootsect.img 512
```

Cool, that created a file that is exactly 512 bytes in size. Let's take a quick look at its contents using the hex dump tool `xxd` (the `-a` option auto-skips nul lines, replacing them with a single `*`):

```
$ xxd -a bootsect.img
00000000: 0000 0000 0000 0000 0000 0000 0000 0000  ................
*
000001f0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
```

The boot sector image is all zeros. Let's try to boot this image:

```
$ qemu bootsect.img
WARNING: Image format was not specified for 'bootsect.img' and probing guessed raw.
         Automatically detecting the format is dangerous for raw images, write operations on block 0 will be restricted.
         Specify the 'raw' format explicitly to remove the restrictions.
```

Let's get rid of this warning by explicitly specifying the disk image format:

```
$ qemu -device file=bootsect.img,format=raw
```
```
Booting from Hard Disk...
Boot failed: not a bootable disk
```

The BIOS doesn't recognize the disk image as bootable. Let's take a look at the Wikipedia page on [Boot sector](https://en.wikipedia.org/wiki/Boot_sector):

> The presence of an IBM PC compatible boot loader for x86-CPUs in the boot sector is by convention indicated by a two-byte hexadecimal sequence 0x55 0xAA (called the boot sector signature) at the end of the boot sector (offsets 0x1FE and 0x1FF). This signature indicates the presence of at least a dummy boot loader which is safe to be executed, even if it may not be able actually to load an operating system.

So that's what we're missing. We need to add those two bytes at the end of the boot sector. Let's use `xxd` again, but this time in _reverse_ mode, i.e. we'll give it the offset and bytes in hex and it will write out them in binary.

```
$ echo "1FE: 55 AA" | xxd -r - bootsect.img
```

This command tells `xxd` to write the two bytes `55 AA` at address `1FE` in the `bootsect.img` file. Let's check the file again:

```
$ xxd -a bootsect.img
00000000: 0000 0000 0000 0000 0000 0000 0000 0000  ................
*
000001f0: 0000 0000 0000 0000 0000 0000 0000 55aa  ..............U.
```

Great! Let's try to boot this image:

```
$ qemu bootsect.img
```
```
Booting from Hard Disk...
```

This time we don't get the error message, which means that the BIOS was able to recognize our image as a valid boot sector.

So what is the computer doing at this point? The CPU is executing the machine code at the start of the boot sector, which is all zeros. The opcode `00` happens to be an `ADD` instruction, so the CPU will basically execute garbage code until it stops due to some unhandled fault.

QEMU has a useful "monitor" view where we can interact with QEMU itself to inspect the current state of the virtual machine (access it through `View > compat_monitor0`). Let's use this feature to see where what the `eip` register (instruction pointer) is pointing to[^1].

```
(qemu) xp $eip
0000000000006805: 0x2607fa84
```
[^1]: You may see a different address/value depending on what options you used to invoke QEMU.

Our boot sector started executing at `0x07C00`, but somehow the CPU end up at some random address.

We need to add valid instructions at the beginning of the boot sector. Normally we'd start coding in assembly at this point, but for fun let's just write the machine code for a simple instruction by hand.

A simple instruction that will serve here is just `hlt`, which halts the CPU. The opcode for `hlt` is `F4`. So let's use `xxd` to set the first byte in the boot sector to this opcode:

```
$ echo "0: F4" | xxd -r - bootsect.img
$ xxd -a bootsect.img
00000000: f400 0000 0000 0000 0000 0000 0000 0000  ................
00000010: 0000 0000 0000 0000 0000 0000 0000 0000  ................
*
000001f0: 0000 0000 0000 0000 0000 0000 0000 55aa  ..............U.
```

Let's boot our image again and inspect the `eip` register:

```
(qemu) xp $eip
0000000000006805: 0x2607fa84
```

We still ended up at the same random address. What's going on here may not be obvious right away, but basically the CPU can get out of the halt state if it receives an interrupt. So we need to disable interrupts before we halt the CPU[^2]. The instruction to disable interrupts is `cli` (for clear interrupt flag), and its opcode is `FA`. So let's modify our boot sector to have those two instructions in sequence (`cli` followed by `hlt`):

[^2]: We can only disable maskable interrupts. Non-Maskable Interrupts (NMI) can always interrupt the CPU. But we won't worry about this now.

```
$ echo "0: FAF4" | xxd -r - bootsect.img
$ xxd -a bootsect.img
00000000: faf4 0000 0000 0000 0000 0000 0000 0000  ................
00000010: 0000 0000 0000 0000 0000 0000 0000 0000  ................
*
000001f0: 0000 0000 0000 0000 0000 0000 0000 55aa  ..............U.
```

Let's boot our image again and inspect the `eip` register:

```
(qemu) xp $eip
0000000000007C02: 0x00000000
```

That address looks more familiar; the CPU stopped right after executing our two instructions. Success!

Let's recap:
* We learned how to create a boot sector disk image for QEMU to use.
* We learned how to add the magic two bytes at the end of the boot sector so that BIOS can recognize it as bootable.
* We learned how to inspect the state of the CPU (specifically the `eip` register) while running in QEMU.
* We learned how to add two machine code instructions to the boot sector to halt the CPU.

Next:
* It's going to be extremely tedious to write machine code by hand. We need to start using an assembler to do that for us.