bits 16

section TEXT class=CODE

global strchr
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

global strcpy
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

global strlen
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

global streq
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
