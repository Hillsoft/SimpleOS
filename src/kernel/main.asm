org 0x0
bits 16

%define ENDL 0x0D, 0x0A

start:
  jmp main

; Prints a string to the screen
; Params:
;   - ds:si points to the target string
puts:
  ; save modified registers
  push si
  push ax

  .loop:
    lodsb
    or al, al
    jz .done

    mov ah, 0x0e
    int 0x10

    jmp .loop

  .done:
  pop ax
  pop si
  ret

main:
  ; print message
  mov si, msg_hello
  call puts

.halt:
  cli
  hlt
  jmp .halt

msg_hello: db 'Kernel started', ENDL, 'Hello, world!', ENDL, 0
