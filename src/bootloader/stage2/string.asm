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

global memset
; void memset(char* dst, char val, uint16_t count)
memset:
  ; new call frame
  push bp
  mov bp, sp

  push di

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - dst
  ; [bp + 6] - val
  ; [bp + 8] - count

  mov ax, [bp + 8]
  mov cl, [bp + 6]
  mov di, [bp + 4]

.loop:
  or ax, ax
  jz .finish

  mov [di], cl
  inc di
  dec ax

  jmp .loop

.finish:
  ; return
  pop di

  mov sp, bp
  pop bp
  ret

global memset_far
; void memset_far(char far* dst, char val, uint16_t count)
memset_far:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push di

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - dst
  ; [bp + 8] - val
  ; [bp + 10] - count

  mov ax, [bp + 10]
  mov cl, [bp + 8]
  mov es, [bp + 6]
  mov di, [bp + 4]

.loop:
  or ax, ax
  jz .finish

  mov es:[di], cl
  inc di
  dec ax

  jmp .loop

.finish:
  ; return
  pop di
  pop es

  mov sp, bp
  pop bp
  ret

global memcpy
; void memcpy(char* dst, const char* src, uint16_t count)
memcpy:
  ; new call frame
  push bp
  mov bp, sp

  push di
  push si

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - dst
  ; [bp + 6] - src
  ; [bp + 8] - count

  mov cx, [bp + 8]
  mov di, [bp + 4]
  mov si, [bp + 6]

.loop:
  or cx, cx
  jz .finish

  mov ax, [si]
  mov [di], ax
  inc si
  inc di
  dec cx
  jmp .loop

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

global memcpy_far
; void memcpy_far(char far* dst, const char far* src, uint16_t count)
memcpy_far:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push fs
  push di
  push si

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - dst
  ; [bp + 8] - src
  ; [bp + 12] - count

  mov cx, [bp + 12]
  mov di, [bp + 4]
  mov es, [bp + 6]
  mov si, [bp + 8]
  mov fs, [bp + 10]

.loop:
  or cx, cx
  jz .finish

  mov al, fs:[si]
  mov es:[di], al
  inc si
  inc di
  dec cx
  jmp .loop

.finish:
  ; return
  pop si
  pop di
  pop fs
  pop es

  mov sp, bp
  pop bp
  ret

global memeq_far
; bool memeq(const char far* a, const char far* b, uint16_t count)
memeq_far:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push fs
  push di
  push si

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - a
  ; [bp + 8] - b
  ; [bp + 12] - count

  mov di, [bp + 4]
  mov es, [bp + 6]
  mov si, [bp + 8]
  mov fs, [bp + 10]
  mov cx, [bp + 12]

.loop:
  or cx, cx
  mov ax, 1
  jz .finish

  mov ax, es:[di]
  mov dx, fs:[si]
  cmp ax, dx
  jne .finish_ne

  dec cx
  inc si
  inc di
  jmp .loop

.finish_ne:
  mov ax, 0

.finish:
  ; return
  pop si
  pop di
  pop fs
  pop es

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
