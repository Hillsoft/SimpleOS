org 0x0
bits 16

%define ENDL 0x0D, 0x0A

;; cdecl calling convention
; arguments: on stack, pushed from left to right, caller clean up
; return: integers and pointers in eax
;         floating point in st0
; scratch: eax, ecx, edx
; saved: all others

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

.halt:
  cli
  hlt
  jmp .halt


; int 10h with ah=0Eh prints a character
; void putc(char c, uint16_t page)
putc:
  ; new call frame
  push bp
  mov bp, sp

  push bx

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - character to print
  ; [bp + 6] - page
  mov ah, 0Eh
  mov al, [bp + 4]
  mov bh, [bp + 6]
  int 10h

  ; return
  pop bx

  mov sp, bp
  pop bp
  ret

; void puts(const char* string)
puts:
  ; new call frame
  push bp
  mov bp, sp

  push bx
  push si

  mov ah, 0Eh
  xor bx, bx

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - char pointer
  mov si, [bp + 4]
.puts.loop:
  lodsb
  or al, al
  jz .puts.strend

  ; inline putc
  int 10h

  ; add si, 1
  jmp .puts.loop

.puts.strend:
  ; return
  pop si
  pop bx

  mov sp, bp
  pop bp
  ret

msg_init: db 'Stage 2 started...', ENDL, 0
