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
