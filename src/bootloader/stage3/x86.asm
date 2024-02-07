global x86_outb
x86_outb:
    [bits 32]
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al
    ret

global x86_inb
x86_inb:
    [bits 32]
    mov dx, [esp + 4]
    xor eax, eax
    in al, dx
    ret

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
