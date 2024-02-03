bits 16

global switchToProtected

section TEXT class=CODE

switchToProtected:
  cli ; disable interrupts
  call enableA20
  call loadGDT

  ; set protection enable flag in CR0
  mov eax, cr0
  or al, 1
  mov cr0, eax

  ; mov eax, pstart
  ; xor ecx, ecx
  ; mov cx, ds
  ; shl ecx, 4
  ; add eax, ecx

  ; far jump into protected mode
  jmp dword 08h:0x500

enableA20:
  ; disable keyboard
  call A20WaitInput
  mov al, KbdControllerDisableKeyboard
  out KbdControllerCommandPort, al

  ; read control output port
  call A20WaitInput
  mov al, KbdControllerReadCtrlOutputPort
  out KbdControllerCommandPort, al

  call A20WaitOutput
  in al, KbdControllerDataPort
  push ax

  ; write control output port
  call A20WaitInput
  mov al, KbdControllerWriteCtrlOutputPort
  out KbdControllerCommandPort, al

  call A20WaitInput
  pop ax
  or al, 2 ; bit 2 is the A20 bit
  out KbdControllerDataPort, al

  ; enable keyboard
  call A20WaitInput
  mov al, KbdControllerEnableKeyboard
  out KbdControllerCommandPort, al

  call A20WaitInput
  ret

A20WaitInput:
  ; wait until status bit 2 is 0
  in al, KbdControllerCommandPort
  test al, 2
  jnz A20WaitInput
  ret

A20WaitOutput:
  ; wait until status bit 1 is 1
  in al, KbdControllerCommandPort
  test al, 1
  jz A20WaitOutput
  ret

loadGDT:
  mov eax, [gdt_descriptor + 2]
  xor ecx, ecx
  mov cx, ds
  shl ecx, 4
  add eax, ecx
  mov [gdt_descriptor + 2], eax

  lgdt [gdt_descriptor]
  ret

SECTION RODATA CLASS=DATA

KbdControllerDataPort equ 0x60
KbdControllerCommandPort equ 0x64

KbdControllerDisableKeyboard equ 0xAD
KbdControllerEnableKeyboard equ 0xAE
KbdControllerReadCtrlOutputPort equ 0xD0
KbdControllerWriteCtrlOutputPort equ 0xD1

gdt_data:
  ; null descriptor
  dq 0

  ; 32-bit code segment
  dw 0FFFFh ; limit
  dw 0 ; base
  db 0
  db 10011010b ; flags
  db 11001111b ; flags + limit
  db 0 ; base high

  ; 32-bit data segment
  dw 0FFFFh ; limit
  dw 0 ; base
  db 0
  db 10010010b ; flags
  db 11001111b ; flags + limit
  db 0 ; base high

  ; 16-bit code segment
  dw 0FFFFh ; limit
  dw 0 ; base
  db 0
  db 10011010b ; flags
  db 00001111b ; flags + limit
  db 0 ; base high

  ; 16-bit data segment
  dw 0FFFFh ; limit
  dw 0 ; base
  db 0
  db 10010010b ; flags
  db 00001111b ; flags + limit
  db 0 ; base high

gdt_descriptor:
  dw gdt_descriptor - gdt_data - 1 ; size
  dd gdt_data
