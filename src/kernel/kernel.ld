SECTIONS
{
    .kernel 0x7e00 :
    {
        build/kernel/kernel.o(.text .rodata .data .bss)
        *(.text) *(.rodata) *(.data) *(.bss)
        . = ALIGN(512);
    }
}
