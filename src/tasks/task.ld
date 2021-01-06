SECTIONS
{
    .task 0xf000 :
    {
        *(.text .data .rodata)
        . = ALIGN(512);
    }
}
