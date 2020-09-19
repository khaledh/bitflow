SECTIONS
{
    .kernel 0x7e00 :
    {
        kernel.o (.text .data)
        . = ALIGN(512);
    }

    __tasks_start = .;
    .tasks :
    {
        task.o (.text .data)
        . = ALIGN(512);
    }
}
