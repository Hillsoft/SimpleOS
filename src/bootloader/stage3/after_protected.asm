bits 32

extern __bss_start
extern __end

extern cstart

section .entry2

pstart:
  mov ax, 0x10
  mov ds, ax
  mov ss, ax

  mov [boot_drive], dl

  ; print hello world
  xor edx, edx
  mov dl, [boot_drive]
  push edx
  call cstart
  add sp, 2

.halt:
  hlt
  jmp .halt

ScreenBuffer equ 0xB8000

boot_drive: db 0
