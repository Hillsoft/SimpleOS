bits 32

extern floppyInterruptHandler

global floppyInterruptHandlerWrapper
floppyInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    call floppyInterruptHandler
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret
