SECTIONS
{
    .kernel 0x7e00 :
    {
        build/kernel/kernel.o(.text .data .rodata .bss)
        *(.text .data .rodata .bss)
        . = ALIGN(512);
    }
}
