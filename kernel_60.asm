;   kernel.asm

    mov    ah, 0x0E              ; tty mode
    mov    al, 'K'
    int    0x10

    cli
    hlt
