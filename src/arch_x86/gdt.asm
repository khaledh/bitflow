;;;
 ; Global Descriptor Table (GDT)
 ;;


gdt_start:         ; 0x00
    ; null descriptor
    dd 0x00000000
    dd 0x00000000

gdt_code:          ; 0x08
    ; code segment descriptor
    dw 0xffff      ; limit (bits 00-15)
    dw 0x0000      ; base address (bits 00-15)
    db 0x00        ; base address (bits 16-23)
    db 10011010b   ; access rights
    ;  ||||| |+----- A:0       not accessed
    ;  ||||+-+------ TYPE:1(C:0)(R:1)  code: (C) non-conforming, (R) readable
    ;  |||+--------- S:1       not system (code, data, or stack)
    ;  |++---------- DPL:00    privilege level 0
    ;  +------------ P:1       present
    db 11001111b   ; flags + limit bits
    ;  |||||  |
    ;  ||||+--+----- limit (bits 16-19)
    ;  |||+--------- AVL:0     available to user
    ;  ||+---------- L:0       not 64-bit code segment
    ;  |+----------- D/B:1     default operand size is 32-bit
    ;  +------------ G:1       limit granularity is 4KB
    db 0x00        ; base address (bits 24-31)

gdt_data:          ; 0x10
    ; data segment descriptor
    dw 0xffff      ; limit (bits 00-15)
    dw 0x0000      ; base address (bits 00-15)
    db 0x00        ; base address (bits 16-23)
    db 10010010b   ; access rights
    ;  ||||| |+----- A:0       (A) not accessed
    ;  ||||+-+------ TYPE:0(E:0)(W:1)  data: (E) not expand-down, (W) writable
    ;  |||+--------- S:1       not system (code, data, or stack)
    ;  |++---------- DPL:00    privilege level 0
    ;  +------------ P:1       present
    db 11001111b   ; flags + limit bits
    ;  |||||  |
    ;  ||||+--+----- limit (bits 16-19)
    ;  |||+--------- AVL:0     available to user
    ;  ||+---------- L:0       not 64-bit code segment
    ;  |+----------- D/B:1     big data segment; offsets are 32-bit
    ;  +------------ G:1       limit granularity is 4KB
    db 0x00        ; base address (bits 24-31)

gdt_descriptor:
  dw $ - gdt_start - 1 ; limit (one less than size)
  dd gdt_start
