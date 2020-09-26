SECTIONS
{
    .kernel 0x7e00 :
    {
        kernel.o(.text .data .rodata)
        *(.text .data .rodata)
        . = ALIGN(512);
    }
}
