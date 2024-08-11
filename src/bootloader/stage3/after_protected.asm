bits 32

section .entry2

pstart:
  cli
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov ss, ax

  mov [boot_drive], dl

  ; disable paging
  mov eax, cr0
  and eax, 01111111111111111111111111111111b
  mov cr0, eax

  ; set up page table
  mov edi, initial_pml4t
  mov cr3, edi
  xor eax, eax

  mov eax, initial_pdpt
  or eax, 3
  mov DWORD [edi], eax

  mov edi, initial_pdpt
  mov eax, initial_pdt
  or eax, 3
  mov DWORD [edi], eax

  mov edi, initial_pdt
  mov eax, initial_pt
  or eax, 3
  mov DWORD [edi], eax

  mov edi, initial_pt
  mov ebx, 0x3
  mov ecx, 512

.setEntry:
  mov DWORD [edi], ebx
  add ebx, 0x1000
  add edi, 8
  loop .setEntry

  ; enable PAE
  mov eax, cr4
  or eax, 1 << 5
  mov cr4, eax

  ; enable paging
  mov ecx, 0xC0000080
  rdmsr
  or eax, 1 << 8
  wrmsr

  mov eax, cr0
  or eax, 1 << 31
  mov cr0, eax

  ; enable SSE
  mov eax, cr0
  and ax, 0xFFFB
  or ax, 0x2
  mov cr0, eax
  mov eax, cr4
  or ax, 3 << 9
  mov cr4, eax

  ; load 64-bit GDT
  lgdt [GDT.Pointer]
  mov dl, [boot_drive]
  jmp GDT.Code:after_x64

ScreenBuffer equ 0xB8000

boot_drive: db 0

; Access bits
PRESENT        equ 1 << 7
NOT_SYS        equ 1 << 4
EXEC           equ 1 << 3
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0

; Flags bits
GRAN_4K       equ 1 << 7
SZ_32         equ 1 << 6
LONG_MODE     equ 1 << 5

GDT:
    .Null: equ $ - GDT
        dq 0
    .Code: equ $ - GDT
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | EXEC | RW            ; Access
        db GRAN_4K | LONG_MODE | 0xF                ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .Data: equ $ - GDT
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | RW                   ; Access
        db GRAN_4K | SZ_32 | 0xF                    ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .TSS: equ $ - GDT
        dd 0x00000068
        dd 0x00CF8900
    .Pointer:
        dw $ - GDT - 1
        dq GDT

initial_pml4t: equ 0x1000
initial_pdpt: equ 0x2000
initial_pdt: equ 0x3000
initial_pt: equ 0x4000

after_x64:
