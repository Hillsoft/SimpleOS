bits 32

extern ps2Port1InterruptHandler

global ps2Port1InterruptHandlerWrapper
ps2Port1InterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    call ps2Port1InterruptHandler
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret

extern ps2Port2InterruptHandler

global ps2Port2InterruptHandlerWrapper
ps2Port2InterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    call ps2Port2InterruptHandler
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret
