bits 16

section TEXT class=CODE

global putc
global puts
global putuint

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

; void putint(uint16_t i, uint16_t base)
; requires:
;   2 <= base <= 16
putuint:
  ; new call frame
  push bp
  mov bp, sp

  push di
  push bx

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - i
  ; [bp + 6] - base

  mov ax, [bp + 4] ; i
  mov cx, [bp + 6] ; base

  ; special handling for zero
  or ax, ax
  jz .zero

  ; we'll use the stack to store digits
  xor di, di ; number of digits

.digitpushloop:
  xor dx, dx
  div word cx ; ax = new i
              ; dx = current digit
  push dx
  inc di

  or ax, ax
  jnz .digitpushloop

.digitprintloop:
  pop bx
  dec di

  mov bx, [bx + digit_map]

  push 0
  push bx
  call putc
  add sp, 4

  or di, di
  jnz .digitprintloop

  jmp .finish

.zero:
  push 0
  push '0'
  call putc
  add sp, 4

.finish:
  ; return
  pop bx
  pop di

  mov sp, bp
  pop bp
  ret

section RODATA class=DATA

digit_map: db '0123456789ABCDEF'
