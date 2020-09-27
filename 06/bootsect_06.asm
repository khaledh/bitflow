;   bootsect.asm

    ;
    ; output 'B' to screen
    ;
    mov    ah, 0x0E              ; tty mode
    mov    al, 'B'
    int    0x10

    ;
    ; load a one-sector dummy kernel
    ;
    mov    ah, 2                 ; INT 13,2 Read Disk Sectors
    mov    al, 1                 ; read one sector
    mov    ch, 0                 ; first track/cylinder
    mov    cl, 2                 ; second sector (sector numbers are 1-based)
    mov    dh, 0                 ; first head
    mov    dl, 0x80              ; drive number, 80h=drive 0
    mov    bx, 0x7E00            ; es:bx = 0x0000:0x7E00 = 0x07E00
    int    0x13

    ;
    ; jump to kernel
    ;
    jmp    word 0x0000:0x7E00

    ;
    ; halt (we should never get here)
    ;
    cli
    hlt

    times 510-($-$$) db 0
    db     0x55, 0xAA
