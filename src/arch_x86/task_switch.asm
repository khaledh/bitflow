global switch_to_thread

extern current_tcb

struc tcb_t
    .esp    resd 1
endstruc


section .text

switch_to_thread:
    pusha

    ; save current thread's state

    mov     edi, [current_tcb]              ; address of current thread's tcb
    mov     [edi + tcb_t.esp], esp          ; save current thread's kernel stack esp

    ; load next thread's state

    mov     esi, [esp + (8+1)*4]            ; address of next thread's tcb
    mov     esp, [esi + tcb_t.esp]          ; load new thread's kernel stack esp

    mov     [current_tcb], esi              ; set current tcb

    popa
    ret                                     ; pop eip from the new thread's kernel stack
