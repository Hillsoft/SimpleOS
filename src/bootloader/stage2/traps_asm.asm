bits 64

%macro pushScratchRegisters 0
    push rax
    push rdi
    push rsi
    push rdx
    push rcx
    push r8
    push r9
    push r10
    push r11
%endmacro

extern divErrorInterruptHandler

global divErrorInterruptHandlerWrapper
divErrorInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rdi, [esp + 72]
    call divErrorInterruptHandler

extern debugInterruptHandler

global debugInterruptHandlerWrapper
debugInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rdi, [esp + 72]
    call debugInterruptHandler

extern nmiInterruptHandler

global nmiInterruptHandlerWrapper
nmiInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rdi, [esp + 72]
    call nmiInterruptHandler

extern breakpointInterruptHandler

global breakpointInterruptHandlerWrapper
breakpointInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rdi, [esp + 72]
    call breakpointInterruptHandler

extern overflowInterruptHandler

global overflowInterruptHandlerWrapper
overflowInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rdi, [esp + 72]
    call overflowInterruptHandler

extern boundRangeExceededInterruptHandler

global boundRangeExceededInterruptHandlerWrapper
boundRangeExceededInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rdi, [esp + 72]
    call boundRangeExceededInterruptHandler

extern invalidOpcodeInterruptHandler

global invalidOpcodeInterruptHandlerWrapper
invalidOpcodeInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rdi, [esp + 72]
    call invalidOpcodeInterruptHandler

extern fpuNotAvailableInterruptHandler

global fpuNotAvailableInterruptHandlerWrapper
fpuNotAvailableInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rdi, [esp + 72]
    call fpuNotAvailableInterruptHandler

extern doubleFaultInterruptHandler

global doubleFaultInterruptHandlerWrapper
doubleFaultInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rsi, [esp + 72]
    mov rdi, [esp + 80]
    call doubleFaultInterruptHandler

extern invalidTssInterruptHandler

global invalidTssInterruptHandlerWrapper
invalidTssInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rsi, [esp + 72]
    mov rdi, [esp + 80]
    call invalidTssInterruptHandler

extern segmentNotPresentInterruptHandler

global segmentNotPresentInterruptHandlerWrapper
segmentNotPresentInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rsi, [esp + 72]
    mov rdi, [esp + 80]
    call segmentNotPresentInterruptHandler

extern stackSegmentFaultInterruptHandler

global stackSegmentFaultInterruptHandlerWrapper
stackSegmentFaultInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rsi, [esp + 72]
    mov rdi, [esp + 80]
    call stackSegmentFaultInterruptHandler

extern generalProtectionFaultInterruptHandler

global generalProtectionFaultInterruptHandlerWrapper
generalProtectionFaultInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rsi, [esp + 72]
    mov rdi, [esp + 80]
    call generalProtectionFaultInterruptHandler

extern pageFaultInterruptHandler

global pageFaultInterruptHandlerWrapper
pageFaultInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    mov rdx, cr2
    mov rsi, [esp + 72]
    mov rdi, [esp + 80]
    call pageFaultInterruptHandler
