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

  push msg_printf_test
  call printf
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

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - char pointer

  mov ah, 0Eh
  xor bx, bx

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

; void printf(const char* fmt, ...)
; supported specifiers:
;   %% - prints '%'
printf:
  ; new call frame
  push bp
  mov bp, sp

  push si
  push di

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - fmt
  ; ... - vargs
  mov di, bp
  add di, 6

  mov si, [bp + 4]

.mainloop:
  lodsb
  or al, al
  jz .strend

  mov cl, '%'
  cmp al, cl
  jne .rawchar

  ; we have a format specifier
  lodsb
  or al, al
  jz .strend

  ; test possibilities
  mov cl, '%'
  cmp al, cl
  je .percent

  jmp .error.badspecifier

.percent:
  push 0
  push '%'
  call putc
  add sp, 4
  jmp .mainloop

.rawchar:
  push 0
  push ax
  call putc
  add sp, 4
  jmp .mainloop

.error.badspecifier:
  push msg_printf_bad_specifier
  call puts
  add sp, 2
  jmp .mainloop

.strend:
  ; return
  pop di
  pop si

  mov sp, bp
  pop bp
  ret

msg_init: db 'Stage 2 started...', ENDL, 0

msg_printf_bad_specifier: db '<bad specifier>', 0

msg_printf_test: db 'percent: %%', ENDL, 0
