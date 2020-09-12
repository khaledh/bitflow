;   kernel.asm

VIDEO_MEMORY         equ 0xb8000
LIGHTGRAY_ON_BLACK   equ 0x07

[bits 32]

    mov    al, 'K'                  ; character to output
    mov    ah, LIGHTGRAY_ON_BLACK   ; character color
    mov    [VIDEO_MEMORY], eax      ; output to start of video memory

    cli
    hlt

    ; times 512-($-$$) db 0
