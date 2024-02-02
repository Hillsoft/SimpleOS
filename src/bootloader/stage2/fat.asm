bits 16

%define ENDL 0x0D, 0x0A
%define FAT_MEMORY_SIZE_HI 0x0001
%define FAT_MEMORY_SIZE_LO 0x0000
%define SECTOR_SIZE 512
%define MAX_OPEN_FILES 10

extern diskReadSectors
extern puts
extern memset_far

section TEXT class=CODE

; struct FAT_File {
;   0: uint16_t handle; index into openFiles array
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
;   ... openFiles
; } size=512
%define FAT_DATA_FAT_OFFSET 512

; struct FAT_FileData {
;   0: FAT_File public;
;   12: bool open;
;   13: uint32_t firstCluster;
;   15: uint32_t currentCluster;
;   19: uint32_t currentSectorInCluster;
;   23: uint8_t buffer[SECTOR_SIZE (i.e. 512)];
; } size=23+512
%define FILE_DATA_OFFSET 23
%define FILE_DATA_SIZE (512 + 23)

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

; uint32_t FAT_cluster_to_lba(uint32_t cluster)
FAT_cluster_to_lba:
  ; new call frame
  push bp
  mov bp, sp

  push bx
  push es

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - cluster

  ; want to calculate data_section_lba + (cluster - 2) * sectors_per_cluster

  mov bx, [fat_data]
  mov es, [fat_data + 2]

  xor cx, cx
  mov cl, es:[bx + 13]
  sub cl, 2

  mov ax, [bp + 4]
  mul word cx
  push ax
  push dx

  mov ax, [bp + 6]
  mul word cx
  pop dx
  add dx, ax
  pop ax

  mov cx, [data_section_lba]
  add ax, cx
  mov cx, [data_section_lba + 2]
  adc dx, cx

  ; return
  pop es
  pop bx

  mov sp, bp
  pop bp
  ret

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
  ; save fat_open_files pointer
  push bx

  add bx, ax
  mov [fat_open_files + 2], es
  mov [fat_open_files], bx

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
  push di

  ;

  ; calculate data_section_lba
  mov es, [fat_data + 2]
  mov bx, [fat_data]
  mov ax, es:[bx + 17] ; dir entries count
  shl ax, 5 ; *= sizeof(DirectoryEntry)

  xor dx, dx
  mov cx, es:[bx + 11] ; bytes per sector
  div word cx

  or dx, dx
  jz .zero
  inc ax
.zero:
  mov [data_section_lba], ax
  mov word [data_section_lba + 2], 0

  ; big memset to ensure 'isopen' is false for all open files
  push MAX_OPEN_FILES * FILE_DATA_SIZE
  push 0
  mov ax, [fat_open_files + 2]
  push ax
  mov ax, [fat_open_files]
  push ax
  call memset_far
  add sp, 8

  mov ax, [fat_open_files + 2]
  push ax
  mov ax, [fat_open_files]
  add ax, FILE_DATA_OFFSET
  push ax

  push 1

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

  or ax, ax
  jnz .finish

  mov es, [fat_open_files + 2]
  mov bx, [fat_open_files]
  mov word es:[bx], 0 ; handle
  mov word es:[bx + 2], 1 ; is directory
  mov word es:[bx + 4], 0 ; position lo
  mov word es:[bx + 6], 0 ; position hi
  mov word es:[bx + 8], di ; size lo
  mov word es:[bx + 10], 0 ; size hi
  mov byte es:[bx + 12], 1 ; is open
  mov word es:[bx + 13], 0 ; first cluster lo
  mov word es:[bx + 15], 0 ; first cluster hi
  mov word es:[bx + 15], 0 ; current cluster lo
  mov word es:[bx + 17], 0 ; current cluster hi
  mov word es:[bx + 19], 0 ; current sector lo
  mov word es:[bx + 21], 0 ; current sector hi

.finish:
  ; return
  pop di
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

; FAT_FileData[]
; index 0 is always the root directory
fat_open_files: dw 0000h, 0050h

data_section_lba: dd 0
