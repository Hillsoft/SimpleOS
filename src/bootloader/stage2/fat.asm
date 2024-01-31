bits 16

%define ENDL 0x0D, 0x0A
%define FAT_MEMORY_SIZE_HI 0x0001
%define FAT_MEMORY_SIZE_LO 0x0000

extern diskReadSectors
extern puts

section TEXT class=CODE

; struct FAT_File {
;   0: uint16_t handle;
;   2: bool isDirectory;
;   4: uint32_t position;
;   8: uint32_t size;
; } size=12

; struct FAT_DirectoryEntry {
;   0: uint8_t name[11];
;   11: uint8_t attributes;
;   12: uint8_t reserved;
;   13: uint8_t createdTimeTenths;
;   14: uint16_t createdTime;
;   16: uint16_t createdDate;
;   18: uint16_t accessedDate;
;   20: uint16_t firstClusterHigh;
;   22: uint16_t modifiedTime;
;   24: uint16_t modifiedDate;
;   26: uint16_t firstClusterLow;
;   28: uint32_t size;
; } size=32

; struct BootSector {
;   0: uint8_t jumpInstruction[3];
;   3: uint8_t oemIdentifier[8];
;   11: uint16_t bytesPerSector;
;   13: uint8_t sectorsPerCluster;
;   14: uint16_t reservedSectors;
;   16: uint8_t fatCount;
;   17: uint16_t dirEntriesCount;
;   19: uint16_t totalSectors;
;   21: uint8_t mediaDescriptorType;
;   22: uint16_t sectorsPerFat;
;   24: uint16_t sectorsPerTrack;
;   26: uint16_t heads;
;   28: uint32_t hiddenSectors;
;   32: uint32_t largeSectorCount;

;   // extended boot record
;   36: uint8_t driveNumber;
;   37: uint8_t reserved;
;   38: uint8_t signature;
;   39: uint32_t volumeId;
;   43: uint8_t volumeLabel[11];
;   54: uint8_t system_id[8];
; } size=62

; struct FAT_Data {
;   0: FAT_BootSector bootSector;
;   -- padding up to 512
;   512: file allocation table
;   ... rootDirectory
; } size=512
%define FAT_DATA_FAT_OFFSET 512

global FAT_initialize
; bool FAT_initialize(DISK* disk)
FAT_initialize:
  ; new call frame
  push bp
  mov bp, sp

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - disk
  mov ax, [bp + 4]
  mov [disk], ax

  call FAT_readBootSector

  or ax, ax
  jz .boot_sector_fail

  call FAT_readFat

  or ax, ax
  jz .generic_fail

  call FAT_readRootDirectory

  or ax, ax
  jz .generic_fail

  mov ax, 1
  jmp .finish

.boot_sector_fail:
  mov ax, msg_boot_sector_read_failed
  push ax
  call puts
  add sp, 2
  mov ax, 0
  jmp .finish

.generic_fail:
  mov ax, 0

.finish:
  ; return
  mov sp, bp
  pop bp
  ret

; global FAT_open
; FAT_File far* FAT_open(const char* path)
FAT_open:

; global FAT_read
; uint32_t FAT_read(FAT_File far* file, uint32_t byteCount, void* dataOut)
FAT_read:

; global FAT_readDirectory
; bool FAT_readDirectory(FAT_File far* file, FAT_DirectoryEntry* directoryEntry)
FAT_readDirectory:

; global FAT_close
; FAT_close(FAT_File far* file)
FAT_close:

; bool FAT_readBootSector()
FAT_readBootSector:
  ; new call frame
  push bp
  mov bp, sp

  mov ax, [fat_data + 2]
  push ax
  mov ax, [fat_data]
  push ax
  push 1
  push 0
  push 0
  mov ax, [disk]
  push ax
  call diskReadSectors
  add sp, 12

  ; return
  mov sp, bp
  pop bp
  ret

; bool FAT_readFat()
FAT_readFat:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push bx

  ; load bytes per sector
  mov es, [fat_data + 2]
  mov bx, [fat_data]
  mov ax, es:[bx + 11]

  ; load sectors per fat
  mov cx, es:[bx + 22]
  mul word cx
  ; dx:ax = fat size

  add ax, FAT_DATA_FAT_OFFSET
  adc dx, 0

  cmp dx, FAT_MEMORY_SIZE_HI
  jg .fat_too_large
  jl .fat_size_fine

  cmp ax, FAT_MEMORY_SIZE_LO
  jge .fat_too_large

.fat_size_fine:
  ; save fat_root_directory pointer
  push bx

  add bx, ax
  mov [fat_root_directory + 2], es
  mov [fat_root_directory], bx

  pop bx

  add bx, FAT_DATA_FAT_OFFSET
  push es
  push bx
  push cx
  xor ax, ax
  push ax
  mov ax, es:[bx + 14]
  push ax
  mov ax, [disk]
  push ax
  call diskReadSectors
  add sp, 12

.finish:
  ; return
  pop bx
  pop es

  mov sp, bp
  pop bp
  ret

.fat_too_large:
  mov ax, msg_fat_too_large
  push ax
  call puts
  add sp, 2

  mov ax, 0
  jmp .finish

; bool FAT_readRootDirectory
FAT_readRootDirectory:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push bx

  ;

  mov ax, [fat_root_directory + 2]
  push ax
  mov ax, [fat_root_directory]
  push ax

  mov es, [fat_data + 2]
  mov bx, [fat_data]
  mov ax, es:[bx + 17] ; dir entries count
  shl ax, 5 ; ax *= sizeof(DirectoryEntry)

  xor dx, dx
  mov cx, es:[bx + 11] ; bytes per sector
  div word cx

  or dx, dx
  jz .zero
  inc ax
.zero:

  push ax

  xor ax, ax
  mov al, es:[bx + 16] ; fat count
  mov dx, es:[bx + 22] ; sectors per fat
  mul word dx
  mov cx, es:[bx + 14]
  add ax, cx
  adc dx, 0
  push dx
  push ax
  mov ax, [disk]
  push ax
  call diskReadSectors
  add sp, 12

  ; return
  pop bx
  pop es

  mov sp, bp
  pop bp
  ret

section RODATA CLASS=DATA

msg_boot_sector_read_failed: db 'FAT: Read boot sector failed', ENDL, 0
msg_fat_too_large: db 'FAT: File allocation table too large', ENDL, 0

; FAT_Data far*
fat_data: dw 0000h, 0050h

section WDATA CLASS=DATA

; DISK*
disk: dw 0

; DirectoryEntry[]
fat_root_directory: dw 0000h, 0050h
