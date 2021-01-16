extern isr_handler

[bits 32]
section .text

    ;
    ; Reference: https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.pdf
    ;
    ; Upon interrupt, the CPU pushes the following registers
    ; before transferring control to the interrupt handler:
    ;
    ;    [ss]          only during a stack switch (i.e. privilege change)
    ;    [esp]         only during a stack switch (i.e. privilege change)
    ;    eflags
    ;    cs
    ;    eip
    ;    [error_code]  only for exceptions that have an error code


; -----------------------------------------------------------------------------
; CPU exceptions
; -----------------------------------------------------------------------------

;
; Divide Error
;
isr00:
    push    0                       ; error code (dummy)
    push    0                       ; interrupt vector
    jmp     isr_common

;
; Debug Exception
;
isr01:
    push    0
    push    1
    jmp     isr_common

;
; Non-Maskable Interrupt (NMI)
;
isr02:
    push    0
    push    2
    jmp     isr_common

;
; Breakpoint (INT 3 instruction)
;
isr03:
    push    0
    push    3
    jmp     isr_common

;
; Overflow (INTO instruction)
;
isr04:
    push    0
    push    4
    jmp     isr_common

;
; BOUND Range Exceeded (BOUND instruction)
;
isr05:
    push    0
    push    5
    jmp     isr_common

;
; Invalid Opcode (Undefined Opcode)
;
isr06:
    push    0
    push    6
    jmp     isr_common

;
; Device Not Available (No Math Coprocessor)
;
isr07:
    push    0
    push    7
    jmp     isr_common

;
; Double Fault
;
isr08:
    push    8
    jmp     isr_common

;
; Coprocessor Segment Overrun
;
isr09:
    push    0
    push    9
    jmp     isr_common

;
; Invalid TSS
;
isr10:
    push    10
    jmp     isr_common

;
; Segment Not Present
;
isr11:
    push    11
    jmp     isr_common

;
; Stack-Segment Fault
;
isr12:
    push    12
    jmp     isr_common

;
; General Protection Fault
;
isr13:
    push    13
    jmp     isr_common

;
; Page Fault
;
isr14:
    push    14
    jmp     isr_common

;
; Vector 15 is reserved.
;

;
; x87 FPU Floating-Point Error (Math Fault)
;
isr16:
    push    0
    push    16
    jmp     isr_common

;
; Alignment Check
;
isr17:
    push    17
    jmp     isr_common

;
; Machine Check
;
isr18:
    push    0
    push    18
    jmp     isr_common

;
; SIMD Floating-Point Exception
;
isr19:
    push    0
    push    19
    jmp     isr_common

;
; Virtualization Exception
;
isr20:
    push    0
    push    20
    jmp     isr_common

; -----------------------------------------------------------------------------
; IRQ interrupts
; -----------------------------------------------------------------------------

isr32:
    push    0
    push    32
    jmp     isr_common

isr33:
    push    0
    push    33
    jmp     isr_common

isr34:
    push    0
    push    34
    jmp     isr_common

isr35:
    push    0
    push    35
    jmp     isr_common

isr36:
    push    0
    push    36
    jmp     isr_common

isr37:
    push    0
    push    37
    jmp     isr_common

isr38:
    push    0
    push    38
    jmp     isr_common

isr39:
    push    0
    push    39
    jmp     isr_common

isr40:
    push    0
    push    40
    jmp     isr_common

isr41:
    push    0
    push    41
    jmp     isr_common

isr42:
    push    0
    push    42
    jmp     isr_common

isr43:
    push    0
    push    43
    jmp     isr_common

isr44:
    push    0
    push    44
    jmp     isr_common

isr45:
    push    0
    push    45
    jmp     isr_common

isr46:
    push    0
    push    46
    jmp     isr_common

isr47:
    push    0
    push    47
    jmp     isr_common


; -----------------------------------------------------------------------------
; Common stub
; -----------------------------------------------------------------------------

isr_common:
    pusha
    push    ds
    push    es
    push    fs
    push    gs

    ; load kernel's segment selectors

    mov     eax, 0x10
    mov     ds, eax
    mov     es, eax
    mov     fs, eax
    mov     gs, eax

    mov     [esp + 28], esp         ; store current esp in its position in stack (as pushed by `pusha` above)
    call    isr_handler

    pop     gs
    pop     fs
    pop     es
    pop     ds
    popa

    add     esp, 0x8                ; pop int_no and error_code

    iret

; -----------------------------------------------------------------------------
; Export symbols
; -----------------------------------------------------------------------------

; Exceptions
global isr00
global isr01
global isr02
global isr03
global isr04
global isr05
global isr06
global isr07
global isr08
global isr09
global isr10
global isr11
global isr12
global isr13
global isr14
; no isr15
global isr16
global isr17
global isr18
global isr19
global isr20

; IRQs
global isr32
global isr33
global isr34
global isr35
global isr36
global isr37
global isr38
global isr39
global isr40
global isr41
global isr42
global isr43
global isr44
global isr45
global isr46
global isr47
