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

extern debugInterruptHandler

global debugInterruptHandlerWrapper
debugInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    call debugInterruptHandler
    add sp, 4
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret

extern nmiInterruptHandler

global nmiInterruptHandlerWrapper
nmiInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    call nmiInterruptHandler
    add sp, 4
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret

extern breakpointInterruptHandler

global breakpointInterruptHandlerWrapper
breakpointInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    call breakpointInterruptHandler
    add sp, 4
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret

extern overflowInterruptHandler

global overflowInterruptHandlerWrapper
overflowInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    call overflowInterruptHandler
    add sp, 4
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret

extern boundRangeExceededInterruptHandler

global boundRangeExceededInterruptHandlerWrapper
boundRangeExceededInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    call boundRangeExceededInterruptHandler
    add sp, 4
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret
