;   bootsect.asm

[org 0x7C00]

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
    mov    al, 16                ; read n sectors
    mov    ch, 0                 ; first track/cylinder
    mov    cl, 2                 ; second sector (sector numbers are 1-based)
    mov    dh, 0                 ; first head
    mov    dl, 0x80              ; drive number, 80h=drive 0
    mov    bx, 0x7E00            ; es:bx = 0x0000:0x7E00 = 0x07E00
    int    0x13

    ;
    ; switch to protected mode
    ;
    cli                          ; disable interrupts
    lgdt   [gdt_descriptor]      ; load the GDT descriptor
    mov    eax, cr0
    or     eax, 0x1              ; set 32-bit mode bit in cr0
    mov    cr0, eax
    jmp    0x08:pm_entry    ; far jump to load code seg selector

[bits 32]
pm_entry:
    ;
    ; we're in protected mode; load data segment registers
    ;
    mov    ax, 0x10              ; update the segment registers
    mov    ds, ax
    mov    es, ax
    mov    fs, ax
    mov    gs, ax
    mov    ss, ax

    ;
    ; jump to kernel
    ;
    jmp    0x08:0x7E00

    ;
    ; halt (we should never get here)
    ;
    cli
    hlt

[bits 16]
%include "src/arch_x86/gdt.asm"

    times 510-($-$$) db 0
    db     0x55, 0xAA
