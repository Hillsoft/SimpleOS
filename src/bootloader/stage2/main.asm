bits 16

%define ENDL 0x0D, 0x0A

;; cdecl calling convention
; arguments: on stack, pushed from left to right, caller clean up
; return: integers and pointers in eax
;         floating point in st0
; scratch: eax, ecx, edx
; saved: all others

; my extension to cdecl will support 32-bit returns as edx:eax

extern puts
extern printf

extern diskInitialize
extern FAT_initialize
extern FAT_open
extern FAT_read

extern strchr
extern strcpy

extern memcpy_far

extern switchToProtected

global entry

section ENTRY class=CODE

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

  mov di, dx

  mov ax, msg_init
  push ax
  call puts
  add sp, 2

  mov ax, msg_init_disk
  push ax
  call puts
  add sp, 2

  mov dx, di
  xor dh, dh
  push dx
  push disk
  call diskInitialize
  add sp, 4

  or ax, ax
  jz .disk_init_fail

  mov ax, msg_init_disk_done
  push ax
  call puts
  add sp, 2

  push disk
  call FAT_initialize
  add sp, 2

  or ax, ax
  jz .fat_init_fail

  mov ax, msg_init_fat_done
  push ax
  call puts
  add sp, 2

  push 230
  push 230
  push test_string
  push 'o'
  push msg_printf_test
  call printf
  add sp, 10

  push '/'
  mov ax, test_strchr
  push ax
  call strchr
  add sp, 4

  push ax
  call puts
  add sp, 2

  push test_strchr
  push test_strcpy
  call strcpy
  add sp, 4

  push test_strcpy
  call puts
  add sp, 2

;   push test_file_name
;   call FAT_open
;   add sp, 2

;   or ax, ax
;   jnz .file_open_success
;   or dx, dx
;   jnz .file_open_success
;   jmp .file_open_fail

; .file_open_success:
;   mov es, dx
;   mov bx, ax

;   push msg_file_open_success
;   call puts
;   add sp, 2

;   push file_out_buffer
;   push 99
;   push es
;   push bx
;   call FAT_read
;   add sp, 8

;   push ax
;   push msg_read_bytes
;   call printf
;   add sp, 4

;   push file_out_buffer
;   call puts
;   add sp, 2

  push stage3_file_name
  call FAT_open
  add sp, 2

  or ax, ax
  jnz .stage3_open_success
  or dx, dx
  jnz .stage3_open_success
  jmp .file_open_fail

.stage3_open_success:
  mov es, dx
  mov bx, ax

  xor di, di

.stage3_read_loop:
  push file_out_buffer
  push 100
  push es
  push bx
  call FAT_read
  add sp, 8

  or ax, ax
  jz .stage3_read_done

  push ax ; to restore

  push ax
  push ds
  push file_out_buffer
  push 50h
  push di
  call memcpy_far
  add sp, 10

  pop ax
  add di, ax

  jmp .stage3_read_loop

.stage3_read_done:

  call switchToProtected

  jmp .halt

.file_open_fail:
  push msg_file_open_fail
  call puts
  add sp, 2
  jmp .halt

.disk_init_fail:
  mov ax, msg_init_disk_fail
  push ax
  call puts
  add sp, 2
  jmp .halt

.fat_init_fail:
  mov ax, msg_fat_init_fail
  push ax
  call puts
  add sp, 2
  jmp .halt

.halt:
  cli
  hlt
  jmp .halt

section RODATA class=DATA

msg_init: db 'Stage 2 started...', ENDL, 0

msg_init_disk: db 'Initialising disc', ENDL, 0

msg_init_disk_done: db 'Disc initialised', ENDL, 0

msg_init_fat_done: db 'FAT initialised', ENDL, 0

msg_printf_test: db 'percent: %%', ENDL
                 db 'char: %c', ENDL
                 db 'string: %s', ENDL
                 db 'uint: %u', ENDL
                 db 'hex: %X', ENDL, 0

test_string: db 'test', 0

msg_init_disk_fail: db 'Failed to initialise disc', ENDL, 0

msg_fat_init_fail: db 'Failed to initialise FAT filesystem', ENDL, 0

msg_file_open_fail: db 'Failed to open file', ENDL, 0

msg_file_open_success: db 'Opened file', ENDL, 0

msg_read_bytes: db 'Read %u bytes from file', ENDL, 0

test_strchr: db 'part1/part2', ENDL, 0

test_strcpy: times 100 db 'a', 0

test_file_name: db 'TEST    TXT', 0

stage3_file_name: db 'STAGE3  BIN', 0

section WDATA class=DATA

disk: times 8 db 0

file_out_buffer: times 100 db 0
