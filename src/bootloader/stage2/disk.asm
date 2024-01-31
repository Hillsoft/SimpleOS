bits 16

section TEXT class=CODE

; struct DISK {
;   0: uint8_t id;
;   1: uint8_t driveType;
;   2: uint16_t cylinders;
;   4: uint16_t sectors;
;   6: uint16_t heads;
; } size=8

global diskInitialize
; bool diskInitialize(DISK* disk, uint8_t driveNumber)
diskInitialize:
  ; new call frame
  push bp
  mov bp, sp

  push si

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - disk
  ; [bp + 6] - drive

  ; save drive->id
  mov si, [bp + 4]
  mov al, [bp + 6]
  mov [si], al

  mov cx, si
  add cx, 6
  push cx
  mov cx, si
  add cx, 4
  push cx
  mov cx, si
  add cx, 2
  push cx
  mov cx, si
  add cx, 1
  push cx
  push ax
  call getDriveParams
  add sp, 10

  ; getDriveParams already returns a bool, just propagate that value on

  ; return
  pop si

  mov sp, bp
  pop bp
  ret

global diskReadSectors
; bool diskReadSectors(DISK* disk, uint32_t lba, uint8_t sectors, uint8_t far* dataOut)
diskReadSectors:
  ; new call frame
  push bp
  mov bp, sp

  push bx
  push si
  push di

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - disk
  ; [bp + 6] - lba
  ; [bp + 10] - sectors
  ; [bp + 12] - dataOut

  sub sp, 6
  ; [sp + 0] - head
  ; [sp + 2] - sector
  ; [sp + 4] - cylinder

  mov bx, sp
  add bx, 4
  mov si, sp
  add si, 2
  mov di, sp

  push di
  push si
  push bx

  mov ax, [bp + 8]
  push ax
  mov ax, [bp + 6]
  push ax
  mov ax, [bp + 4]
  push ax
  call diskLBA2CHS
  add sp, 12

  mov bx, [bx]
  mov si, [si]
  mov di, [di]

  ; [sp + 0] - loop index
  mov dx, di
  mov di, sp
  mov cx, 2
  mov [di], cx
  mov di, dx

.readLoop:
  mov ax, [bp + 14]
  push ax
  mov ax, [bp + 12]
  push ax
  mov ax, [bp + 10]
  push ax
  push di
  push si
  push bx
  mov dx, di
  mov di, [bp + 4]
  mov di, [di]
  push di
  mov di, dx
  call diskRead
  add sp, 14

  ; ax return success bit
  or ax, ax
  jnz .finished

  mov dx, di
  mov di, sp
  mov cx, [di]
  dec cx
  or cx, cx
  jz .finished

  mov [di], cx
  mov di, dx
  jmp .readLoop

.finished:
  ; return
  add sp, 6

  pop di
  pop si
  pop bx

  mov sp, bp
  pop bp
  ret

; bool diskReset(uint8_t drive)
; returns true for success, false for error
diskReset:
  ; new call frame
  push bp
  mov bp, sp

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - drive

  mov dl, [bp + 4]
  mov ah, 0
  stc
  int 13h
  ; error flag is in carry bit

  mov ax, 1
  sbb ax, 0

  ; return
  mov sp, bp
  pop bp
  ret

; bool diskRead(uint8_t drive, uint16_t cylinder, uint16_t sector, uint16_t head, uint8_t count, uint8_t far* dataOut)
; returns true for success, false for error
diskRead:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push bx

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - drive
  ; [bp + 6] - cylinder
  ; [bp + 8] - sector
  ; [bp + 10] - head
  ; [bp + 12] - count
  ; [bp + 14] dataOut

  mov ah, 02h
  mov ch, [bp + 6]

  mov cl, [bp + 7] ; cylinder bits 6-7
  shl cl, 6
  mov al, [bp + 8]
  or cl, al ; cl - merged sector and cylinder high bits

  mov al, [bp + 12]
  mov dh, [bp + 10]
  mov dl, [bp + 4]

  mov bx, [bp + 16]
  mov es, bx
  mov bx, [bp + 14]

  stc
  int 13h
  ; error flag is in carry bit

  mov ax, 1
  sbb ax, 0

  ; return
  pop bx
  pop es

  mov sp, bp
  pop bp
  ret


; bool getDriveParams(uint8_t drive, uint8_t* driveTypeOut, uint16_t* cylindersOut, uint16_t* sectorsOut, uint16_t* headsOut)
; returns true for success, false for error
getDriveParams:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push di
  push si
  push bx

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - drive
  ; [bp + 6] - driveTypeOut
  ; [bp + 8] - cylindersOut
  ; [bp + 10] - sectorsOut
  ; [bp + 12] - headsOut

  mov ah, 08h
  mov dl, [bp + 4]
  mov di, 0
  mov es, di
  stc
  int 13h
  ; error flag is in carry bit

  mov ax, 1
  sbb ax, 0

  ; drive type
  mov si, [bp + 6]
  mov [si], bl

  ; cylinders
  mov bl, ch ; low bits from ch
  mov bh, cl
  shr bh, 6 ; high bits from cl
  mov si, [bp + 8]
  mov [si], bx

  ; sectors
  xor ch, ch
  and cl, 0b00111111
  mov si, [bp + 10]
  mov [si], cx

  ; heads
  mov cl, dh
  mov si, [bp + 12]
  mov [si], cx

  ; return
  pop bx
  pop si
  pop di
  pop es

  mov sp, bp
  pop bp
  ret

; void diskLBA2CHS(DISK* disk, uint32_t lba, uint16_t* cylindersOut, uint16_t* sectorsOut, uint16_t* headsOut)
diskLBA2CHS:
  ; new call frame
  push bp
  mov bp, sp

  push di
  push si

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - disk
  ; [bp + 6] - lba
  ; [bp + 10] - cylindersOut
  ; [bp + 12] - sectorsOut
  ; [bp + 14] - headsOut

  ; sector = (LBA % sectors per track + 1)
  mov ax, [bp + 6]
  mov dx, [bp + 8]

  mov si, [bp + 4]
  mov cx, [si + 4]

  div word cx ; ax = LBA / sectors
              ; dx = LBA % sectors
  add dx, 1

  mov di, [bp + 12]
  mov [di], dx

  ; cylinder = (LBA / sectors per track) / heads
  xor dx, dx
  mov cx, [si + 6]
  div word cx ; ax = (LBA / sectors) / heads
              ; dx = (LBA / sectors) % heads

  mov di, [bp + 10]
  mov [di], ax

  ; head = (LBA / sectors per track) % heads
  mov di, [bp + 14]
  mov [di], dx

  ; return
  pop si
  pop di

  mov sp, bp
  pop bp
  ret
