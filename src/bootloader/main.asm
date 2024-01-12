org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

;
; FAT12 Header
;

jmp short start
nop

bdb_oem:                    db 'SimpleOS'       ; 8 bytes
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 0E0h             ; 224
bdb_total_sectors:          dw 2880
bdb_media_descriptor_type:  db 0F0h
bdb_sectors_per_fat:        dw 9
bdb_sectors_per_track:      dw 18
bdb_heads:                  dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0

; extended boot record
ebr_drive_number:           db 0
                            db 0                ; reserved
ebr_signature:              db 029h
ebr_volume_id:              db 12, 8, 73, 6
ebr_volume_label:           db 'SimplOSDsk '    ; 11 bytes
ebr_system_id:              db 'FAT12   '       ; 8 bytes


start:
  ; setup data segments
  mov ax, 0
  mov ds, ax
  mov es, ax

  ; setup stack
  mov ss, ax
  mov sp, 0x7C00

  ; some BIOSes might start us at 07C0:0000
  ; ensure we are at 0000:7C00
  push es
  push word .after
  retf
.after:

  ; read from disk
  ; BIOS should set DL to drive number
  mov [ebr_drive_number], dl

  mov ax, 1       ; LBA = 1
  mov cl, 1       ; sector count
  mov bx, 0x7E00  ; address to store result
  call disk_read

  mov si, msg_disk_loaded
  call puts

  hlt

.halt:
  cli
  hlt
  jmp .halt

; Prints a string to the screen
; Params:
;   - ds:si points to the target string
puts:
  ; save modified registers
  push si
  push ax

  .loop:
    lodsb
    or al, al
    jz .done

    mov ah, 0x0e
    int 0x10

    jmp .loop

  .done:
  pop ax
  pop si
  ret

;
; Disk operations
;

;
; Convert LBA address to CHS address
; Parameters:
;   - ax: LBA address
; Returns:
;   - cx[0-5]: sector number
;   - cx[6-15]: cylinder
;   - dh: head
lba_to_chs:
  push ax
  push dx

  xor dx, dx                          ; dx = 0
  div word [bdb_sectors_per_track]    ; ax = LBA / sectors per track
                                      ; dx = LBA % sectors per track
  inc dx
  mov cx, dx                          ; cx = sector

  xor dx, dx
  div word [bdb_heads]                ; ax = (LBA / sectors per track) / heads
                                      ; dx = (LBA / sectors per track) % heads
  mov dh, dl
  mov ch, al
  shl ah, 6
  or cl, ah

  pop ax
  mov dl, al
  pop ax
  ret

;
; Reads sectors from disc
; Parameters:
;   - ax: LBA address
;   - cl: number of sectors (up to 128)
;   - dl: drive number
;   - ex:bx: address to store data in
disk_read:
  push ax
  push cx
  push dx
  push di

  push cx
  call lba_to_chs
  pop ax

  mov ah, 02h

  mov di, 3     ; retry count

  .retry:
    pusha
    stc
    int 13h
    jnc .done   ; carry cleared, success!

    ; read failed
    popa
    call disk_reset
    dec di
    test di, di
    jnz .retry

  .fail:
    ; attempts failed
    jmp disk_error

  .done:
  popa

  pop di
  pop dx
  pop cx
  pop ax
  ret

;
; Reset disc controller
; Parameters:
;   - dl: drive number
disk_reset:
  pusha
  mov ah, 0
  stc
  int 13h
  jc disk_error
  popa
  ret

disk_error:
  mov si, msg_disk_error
  call puts

  mov ah, 0
  int 16h       ; wait for keypress
  jmp 0FFFFh:0


msg_disk_loaded: db 'Loaded data from disc', ENDL, 0
msg_disk_error: db 'Read from disc failed', ENDL, 0


times 510-($-$$) db 0
dw 0AA55h
