bits 16

extern puts
extern putc
extern putuint

section TEXT class=CODE

global printf

; void printf(const char* fmt, ...)
; supported specifiers:
;   %% - prints '%'
;   %c - prints char
;   %s - prints string
;   %u - prints unsigned decimal int
;   %X - prints unsigned hexadecimal (capitals)
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

  mov cl, 'c'
  cmp al, cl
  je .char

  mov cl, 's'
  cmp al, cl
  je .string

  mov cl, 'u'
  cmp al, cl
  je .uint

  mov cl, 'X'
  cmp al, cl
  je .uhexcaps

  jmp .error.badspecifier

.percent:
  push 0
  push '%'
  call putc
  add sp, 4
  jmp .mainloop

.char:
  mov dx, [di]
  add di, 2

  push 0
  push dx
  call putc
  add sp, 4
  jmp .mainloop

.string:
  mov dx, [di]
  add di, 2

  push dx
  call puts
  add sp, 2
  jmp .mainloop

.uint:
  mov dx, [di]
  add di, 2

  push 10
  push dx
  call putuint
  add sp, 4
  jmp .mainloop

.uhexcaps:
  mov dx, [di]
  add di, 2

  push 16
  push dx
  call putuint
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

section RODATA class=DATA

msg_printf_bad_specifier: db '<bad specifier>', 0
