bits 32

extern __bss_start
extern __end

extern getHello

section .entry2

pstart:
  mov ax, 0x10
  mov ds, ax
  mov ss, ax

  mov [boot_drive], dl

  ; clear bss
  mov edi, __bss_start
  mov ecx, __end
  sub ecx, edi
  mov al, 0
  cld
  rep stosb

  ; print hello world
  call getHello

  mov esi, eax
  mov edi, ScreenBuffer
  cld

.loop:
  lodsb
  or al, al
  jz .done

  mov [edi], al

  inc edi
  inc edi

  jmp .loop

.done:

.halt:
  hlt
  jmp .halt

ScreenBuffer equ 0xB8000

boot_drive: db 0

msg_hello: db 'Hello, we are in protected mode!', 0
