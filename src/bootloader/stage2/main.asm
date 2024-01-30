bits 16

%define ENDL 0x0D, 0x0A

;; cdecl calling convention
; arguments: on stack, pushed from left to right, caller clean up
; return: integers and pointers in eax
;         floating point in st0
; scratch: eax, ecx, edx
; saved: all others

extern puts
extern printf

section ENTRY class=CODE

global entry

; we receive boot drive in dl
entry:
  ; set up stack
  cli
  mov ax, ds
  mov ss, ax
  mov sp, 0
  mov bp, sp
  sti

  xor dh, dh

  mov ax, msg_init
  push ax
  call puts
  add sp, 2

  push 230
  push 230
  push test_string
  push 'o'
  push msg_printf_test
  call printf
  add sp, 10

.halt:
  cli
  hlt
  jmp .halt

section RODATA class=DATA

msg_init: db 'Stage 2 started...', ENDL, 0

msg_printf_test: db 'percent: %%', ENDL
                 db 'char: %c', ENDL
                 db 'string: %s', ENDL
                 db 'uint: %u', ENDL
                 db 'hex: %X', ENDL, 0

test_string: db 'test', 0
