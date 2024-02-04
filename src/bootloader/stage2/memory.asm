bits 16

global memset
global memset_far
global memcpy
global memcpy_far
global memeq_far

section TEXT class=CODE

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
