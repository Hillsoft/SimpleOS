bits 32

extern divErrorInterruptHandler

global divErrorInterruptHandlerWrapper
divErrorInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    call divErrorInterruptHandler
    add sp, 4
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret
