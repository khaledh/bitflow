When a PC is turned on, the BIOS performs a Power-On Self-Test (POST), and then starts the CPU.

The CPU starts executing at address 0xFFFFFFF0. The BIOS ROM entry point is mapped at this memory address, so control is transferred to the BIOS.

After the BIOS configures hardware devices, it begins looking for a bootable device. Typically this is a hard drive, a floppy drive, a CD-ROM, or a network boot ROM.

Assuming the first bootable device is a hard drive, the BIOS loads the first sector of that device into memory location 0x7C00 and transfers control to it.

At this point the execution environment is called Real-Address Mode, or Real Mode for short. This is a 16-bit mode. Since a 16-bit address can access only 64K of memory, a scheme called segmentation is used to allow addressing up to 1MB, which is the maximum allowed by the 20-bit address bus of the original 8086. A segmented memory address consists of two parts: a 16-bit segment and a 16-bit offset within that segment. To form a physical address, the segment is multiplied by 16 (i.e. left-shifted by 4-bits) and the offset is added to it.

Example:
       segment:  0x1000
        offset:  0xFD33
  real address: 0x1FD33

So we have a memory address space ranging from 0x00000 to 0xFFFFF (1MB).

Let's take a look at the memory layout at this point (note that the memory starts at the bottom, following the Intel manuals conventions):

```
-------------------------------------------------------------------------------
            start   - end      size   desription
-------------------------------------------------------------------------------
 1,048,576 0x100000 -                 Unaddressable memory in real mode
 1,048,574  0xFFFFE - 0xFFFFF (   2B) BIOS Entry Point
   983,040  0xF0000 - 0xFFFFD ( ~64K) BIOS ROM
   819,200  0xC8000 - 0xEFFFF ( 160K) Memory-mapped ISA Hardware (or no memory)
   786,432  0xC0000 - 0xC7FFF (  32K) Video ROM
   753,664  0xB8000 - 0xBFFFF (  32K) Video RAM for Color Text Mode
   720,896  0xB0000 - 0xB7FFF (  32K) Video RAM for Monochrome Text Mode
   655,360  0xA0000 - 0xAFFFF (  64K) Video RAM for Graphics Mode
------------------------- Top of RAM ------------------------------------------
   654,336  0x9FC00 - 0x9FFFF (   1K) Extended BIOS Data Area (EBDA)
    32,256  0x07E00 - 0x9FBFF (~622K) Free memory
    31,744  0x07C00 - 0x07DFF ( 512B) Boot Sector
     1,280  0x00500 - 0x07BFF ( ~30K) Free memory
     1,024  0x00400 - 0x004FF ( 256B) BIOS Data Area (BDA)
         0  0x00000 - 0x003FF (   1K) Interrupt Vector Table (IVT)
-------------------------------------------------------------------------------
```
