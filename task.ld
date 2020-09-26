SECTIONS
{
    .task :
    {
        *(.text .data .rodata)
        . = ALIGN(512);
    }
}
