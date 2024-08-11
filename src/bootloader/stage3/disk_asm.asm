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

%macro popScratchRegisters 0
    pop r11
    pop r10
    pop r9
    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rax
%endmacro

extern floppyInterruptHandler

global floppyInterruptHandlerWrapper
floppyInterruptHandlerWrapper:
    cli
    pushScratchRegisters
    call floppyInterruptHandler
    mov al, 20h
    out 20h, al
    popScratchRegisters
    sti
    iretq
