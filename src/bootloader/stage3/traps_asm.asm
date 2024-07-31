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

extern invalidOpcodeInterruptHandler

global invalidOpcodeInterruptHandlerWrapper
invalidOpcodeInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    call invalidOpcodeInterruptHandler
    add sp, 4
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret

extern fpuNotAvailableInterruptHandler

global fpuNotAvailableInterruptHandlerWrapper
fpuNotAvailableInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    call fpuNotAvailableInterruptHandler
    add sp, 4
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    sti
    iret

extern doubleFaultInterruptHandler

global doubleFaultInterruptHandlerWrapper
doubleFaultInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    mov eax, [esp + 16]
    push eax
    call doubleFaultInterruptHandler
    add sp, 8
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    add sp, 4
    sti
    iret

extern invalidTssInterruptHandler

global invalidTssInterruptHandlerWrapper
invalidTssInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    mov eax, [esp + 16]
    push eax
    call invalidTssInterruptHandler
    add sp, 8
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    add sp, 4
    sti
    iret

extern segmentNotPresentInterruptHandler

global segmentNotPresentInterruptHandlerWrapper
segmentNotPresentInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    mov eax, [esp + 16]
    push eax
    call segmentNotPresentInterruptHandler
    add sp, 8
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    add sp, 4
    sti
    iret

extern stackSegmentFaultInterruptHandler

global stackSegmentFaultInterruptHandlerWrapper
stackSegmentFaultInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    mov eax, [esp + 16]
    push eax
    call stackSegmentFaultInterruptHandler
    add sp, 8
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    add sp, 4
    sti
    iret

extern generalProtectionFaultInterruptHandler

global generalProtectionFaultInterruptHandlerWrapper
generalProtectionFaultInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    mov eax, [esp + 16]
    push eax
    call generalProtectionFaultInterruptHandler
    add sp, 8
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    add sp, 4
    sti
    iret

extern pageFaultInterruptHandler

global pageFaultInterruptHandlerWrapper
pageFaultInterruptHandlerWrapper:
    cli
    push eax
    push ecx
    push edx
    mov eax, [esp + 12]
    push eax
    mov eax, [esp + 16]
    push eax
    call pageFaultInterruptHandler
    add sp, 8
    mov al, 20h
    out 20h, al
    pop edx
    pop ecx
    pop eax
    add sp, 4
    sti
    iret
