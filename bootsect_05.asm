;   bootsect.asm

    mov    ah, 0x0E              ; tty mode
    mov    al, 'B'
    int    0x10

    cli
    hlt

    times 510-($-$$) db 0
    db     0x55, 0xAA
