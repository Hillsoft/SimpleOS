bits 16

global strchr
global strcpy
global strlen
global streq
global islower
global toupper

section TEXT class=CODE

; const char* strchr(const char* str, char chr)
strchr:
  ; new call frame
  push bp
  mov bp, sp

  push si

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - str
  ; [bp + 6] - chr

  mov si, [bp + 4]
  mov cx, [bp + 6]

.loop:
  lodsb
  or al, al
  jz .strend

  cmp al, cl
  jne .loop

.match:
  mov ax, si
  dec ax
  jmp .finish

.strend:
  xor ax, ax

.finish:
  ; return
  pop si

  mov sp, bp
  pop bp
  ret

; char* strcpy(char* dst, const char* src)
strcpy:
  ; new call frame
  push bp
  mov bp, sp

  push di
  push si

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - dst
  ; [bp + 6] - src

  mov di, [bp + 4]
  or di, di
  jz .nulldest

  mov si, [bp + 6]
  or si, si
  jz .nullsrc

  mov cx, di

.loop:
  lodsb
  mov [di], al
  inc di

  or al, al
  jnz .loop

  mov ax, di
  jmp .finish

.nullsrc:
  mov byte [di], 0
  mov ax, di
  jmp .finish

.nulldest:
  xor ax, ax

.finish:
  ; return
  pop si
  pop di

  mov sp, bp
  pop bp
  ret

; uint16_t strlen(const char* str)
strlen:
  ; new call frame
  push bp
  mov bp, sp

  push si

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - str

  xor cx, cx
  mov si, [bp + 4]
.loop:
  lodsb
  or ax, ax
  jz .finish

  inc cx
  jmp .loop

.finish:
  mov ax, cx

  ; return
  pop si

  mov sp, bp
  pop bp
  ret

; bool streq(const char* a, const char* b)
streq:
  ; new call frame
  push bp
  mov bp, sp

  push di
  push si

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - a
  ; [bp + 6] - b

  mov di, [bp + 4]
  mov si, [bp + 6]

.loop:
  mov ax, [di]
  mov cx, [si]
  cmp ax, cx
  jne .finish_ne

  or ax, ax
  mov ax, 1
  jz .finish

  inc si
  inc di
  jmp .loop

.finish_ne:
  mov ax, 0

.finish:
  ; return
  pop si
  pop di

  mov sp, bp
  pop bp
  ret

; bool islower(char c)
islower:
  ; new call frame
  push bp
  mov bp, sp

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - c

  xor ax, ax

  mov cx, [bp + 4]
  mov dx, 'a'

  cmp cx, dx
  jl .finish

  mov dx, 'z'
  cmp cx, dx
  jg .finish

  mov ax, 1

.finish:
  ; return
  mov sp, bp
  pop bp
  ret

; char toupper(char c)
toupper:
  ; new call frame
  push bp
  mov bp, sp

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - c

  mov ax, [bp + 4]
  push ax
  call islower
  add sp, 2

  mov cx, [bp + 4]
  mov dx, cx
  add dx, 'A' - 'a'

  or ax, ax
  cmovnz cx, dx

  mov ax, cx

  ; return
  mov sp, bp
  pop bp
  ret
