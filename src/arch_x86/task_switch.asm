;;;
 ; Low-level Task Switching
 ;
 ; This is used only for starting the very first task from kmain.
 ; All subsequent context switches happen in isr_common (isr.asm), which
 ; saves/restores current_task->esp around the call to isr_handler.
 ;;

struc task_t
    .esp    resd 1
endstruc

section .text

global resume_new_task

; resume_new_task(task_t* task)
; Used to start the first task from kmain. Loads the task's esp and
; executes the iret epilogue to enter the task.
resume_new_task:
    mov     edi, [esp + 4]                ; task
    mov     esp, [edi + task_t.esp]       ; load task's kernel stack esp

    ; The stack now points to the new task's frame built by create_task/create_user_task:
    pop     gs
    pop     fs
    pop     es
    pop     ds
    popa
    add     esp, 0x8                      ; pop int_no and error_code
    iret
