;;;
 ; Low-level Task Switching
 ;;

extern current_task

struc task_t
    .esp    resd 1
endstruc


section .text

global switch_to_task
switch_to_task:
    pusha

    ; save current task's state

    mov     edi, [current_task]              ; address of current task's struct
    mov     [edi + task_t.esp], esp          ; save current task's kernel stack esp

    ; load next task's state

    mov     esi, [esp + (8+1)*4]             ; address of next task's struct
    mov     [current_task], esi              ; set current task struct

    mov     esp, [esi + task_t.esp]          ; load new task's kernel stack esp

    popa
    ret                                      ; pop eip from the new task's kernel stack


global switch_to_user_task
switch_to_user_task:
    pusha
    push    ds
    push    es
    push    fs
    push    gs

    ; save current task's state

    mov     edi, [current_task]              ; address of current task's struct
    mov     [edi + task_t.esp], esp          ; save current task's stack esp

    ; load next task's state

    mov     esi, [esp + (8+1)*4]             ; address of next task's struct
    mov     [current_task], esi              ; set current task struct

    mov     esp, [esi + task_t.esp]          ; load new task's kernel stack esp

    pop     gs
    pop     fs
    pop     es
    pop     ds
    popa
    ret                                      ; pop eip from the new task's kernel stack


global switch_to_new_task
switch_to_new_task:
    pusha

    ; save current task's state

    mov     edi, [current_task]              ; address of current task's struct
    mov     [edi + task_t.esp], esp          ; save current task's kernel stack esp

    ; load next task's state

    mov     esi, [esp + (8+1)*4]             ; address of next task's struct
    mov     [current_task], esi              ; set current task struct

    mov     esp, [esi + task_t.esp]          ; load new task's kernel stack esp

    pop     gs
    pop     fs
    pop     es
    pop     ds
    popa

    add     esp, 0x8                         ; pop int_no and error_code

    iret
