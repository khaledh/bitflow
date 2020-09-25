SECTIONS
{
    .kernel 0x7e00 :
    {
        kernel.o(.text)
        EXCLUDE_FILE (task_*.o) *(.text)
        kernel.o(.data .rodata)
        EXCLUDE_FILE (task_*.o) *(.data .rodata)
        . = ALIGN(512);
    }

    __tasks_start = .;
    .tasks :
    {
        task_a.o (.text .data .rodata)
        . = ALIGN(512);

        task_b.o (.text .data .rodata)
        . = ALIGN(512);
    }
}
