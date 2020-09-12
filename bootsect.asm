;   bootsect.asm

    cli
    hlt

    times 508 db 0
    db 0x55, 0xAA
