org 0x500

bits 32

pstart:
  mov ax, 0x10
  mov ds, ax
  mov ss, ax

  ; print hello world
  mov esi, msg_hello
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

msg_hello: db 'Hello, we are in protected mode!', 0
