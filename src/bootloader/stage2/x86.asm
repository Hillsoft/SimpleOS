bits 64

global x86_load_interrupt_table
x86_load_interrupt_table:
    lidt [rdi]
    ret

global x86_trivial_interrupt
align 4
x86_trivial_interrupt:
    push rax
    mov al, 20h
    out 20h, al
    pop rax
    iretq
