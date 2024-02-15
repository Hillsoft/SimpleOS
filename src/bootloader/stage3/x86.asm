global x86_load_interrupt_table
x86_load_interrupt_table:
    [bits 32]
    mov eax, [esp + 4]
    lidt [eax]
    ret

global x86_trivial_interrupt
align 4
x86_trivial_interrupt:
    [bits 32]
    push eax
    mov al, 20h
    out 20h, al
    pop eax
    iret
