bits 16

%define ENDL 0x0D, 0x0A
%define FAT_MEMORY_SIZE_HI 0x0001
%define FAT_MEMORY_SIZE_LO 0x0000
%define SECTOR_SIZE 512
%define MAX_OPEN_FILES 10
%define MAX_PATH_SIZE 256

extern diskReadSectors
extern puts
extern memcpy_far
extern memset_far
extern memeq_far

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

%define FAT_ATTRIBUTE_READ_ONLY 01h
%define FAT_ATTRIBUTE_HIDDEN 02h
%define FAT_ATTRIBUTE_SYSTEM 04h
%define FAT_ATTRIBUTE_VOLUME_ID 08h
%define FAT_ATTRIBUTE_DIRECTORY 10h
%define FAT_ATTRIBUTE_ARCHIVE 20h

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

global FAT_open
; FAT_File far* FAT_open(const char* path)
FAT_open:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push bx
  push si

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - path

  sub sp, MAX_PATH_SIZE

  ; [sp + 0] - path_buffer (MAX_PATH_SIZE bytes)

  mov si, [bp + 4]

  ; ignore leading slash
  mov ax, [si]
  cmp ax, '/'
  jne .leading_slash_check_done
  inc si
.leading_slash_check_done:

  push si
  call FAT_find_file
  add sp, 2
  or ax, ax
  jnz .found_file
  or dx, dx
  jnz .found_file
  jmp .finish

.found_file:
  push dx
  push ax
  call FAT_open_directory_entry
  add sp, 4

.finish:
  ; return
  add sp, MAX_PATH_SIZE

  pop si
  pop bx
  pop es

  mov sp, bp
  pop bp
  ret

; FAT_DirectoryEntry far* FAT_find_file(const char* name)
FAT_find_file:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push bx

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - name

  mov es, [fat_data + 2]
  mov bx, [fat_data]
  mov cx, es:[bx + 17] ; dirEntryCount

  mov es, [fat_open_files + 2]
  mov bx, [fat_open_files]
  add bx, FILE_DATA_OFFSET

.loop:
  or cx, cx
  jz .not_found

  push cx
  mov ax, [bp + 4]
  push 11
  push es
  push bx
  push ds
  push ax
  call memeq_far
  add sp, 10
  pop cx

  or ax, ax
  jnz .found

  dec cx
  jmp .loop

.not_found:
  xor dx, dx
  xor ax, ax
  jmp .finish

.found:
  mov dx, es
  mov ax, bx

.finish:
  ; return
  pop bx
  pop es

  mov sp, bp
  pop bp
  ret

; FAT_File far* FAT_open_directory_entry(FAT_DirectoryEntry far* entry)
FAT_open_directory_entry:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push bx
  push cs
  push si
  push di

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - entry

  mov es, [fat_open_files + 2]
  mov bx, [fat_open_files]

  xor ax, ax

.file_search_loop:
  mov cl, es:[bx + 12]
  or cl, cl

  jz .found_file

  add bx, FILE_DATA_SIZE
  inc ax

  cmp ax, MAX_OPEN_FILES
  jl .file_search_loop

  push msg_too_many_open_files
  call puts
  add sp, 2

  xor ax, ax
  xor dx, dx
  jmp .finish

.found_file:
  ; ax = file handle
  ; es:bx = file pointer

  mov fs, [bp + 6]
  mov si, [bp + 4]

  mov word es:[bx], ax ; handle

  xor dx, dx
  mov di, 1
  mov cl, fs:[si + 11]
  and cl, FAT_ATTRIBUTE_DIRECTORY
  cmovnz dx, di
  mov word es:[bx + 2], dx ; is directory

  mov word es:[bx + 4], 0 ; position lo
  mov word es:[bx + 6], 0 ; position hi

  mov ax, fs:[si + 28]
  mov cx, fs:[si + 30]
  mov word es:[bx + 8], ax ; size lo
  mov word es:[bx + 10], cx ; size hi

  mov ax, fs:[si + 26]
  mov cx, fs:[si + 20]
  mov word es:[bx + 13], ax ; first cluster lo
  mov word es:[bx + 15], cx ; first cluster hi

  mov word es:[bx + 15], ax ; current cluster lo
  mov word es:[bx + 17], cx ; current cluster hi

  mov word es:[bx + 19], 0 ; current sector lo
  mov word es:[bx + 21], 0 ; current sector hi

  push cx
  push ax
  call FAT_cluster_to_lba
  add sp, 4

  push es
  add bx, 23
  push bx
  push 1
  push dx
  push ax
  mov cx, [disk]
  push cx
  call diskReadSectors
  add sp, 12

  xor dx, dx
  or ax, ax
  jz .finish

  sub bx, 23

  mov byte es:[bx + 12], 1 ; is open
  mov ax, bx
  mov dx, es

.finish:
  ; return
  pop di
  pop si
  pop fs
  pop bx
  pop es

  mov sp, bp
  pop bp
  ret

; uint16_t FAT_next_cluster(uint16_t currentCluster)
FAT_next_cluster:
  ; new call frame
  push bp
  mov bp, sp

  push es
  push bx

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - currentCluster

  mov es, [fat_data + 2]
  mov bx, [fat_data]
  add bx, FAT_DATA_FAT_OFFSET

  mov ax, [bp + 4]
  mov cx, ax
  shl ax, 1
  add ax, cx
  shr ax, 1 ; fatIndex = currentCluster * 3 / 2

  mov cx, 1
  and cx, ax
  jz .even_cluster

.odd_cluster:
  add bx, ax
  mov ax, es:[bx]
  shr ax, 4
  jmp .done

.even_cluster:
  add bx, ax
  mov ax, es:[bx]
  add ax, 0x0FFF
  jmp .done

.done:
  ; return
  pop bx
  pop es

  mov sp, bp
  pop bp
  ret

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

  mov ax, [bp + 4]
  mov dx, [bp + 6]
  sub ax, 2
  sbb dx, 0
  mov [bp + 6], dx

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

global FAT_read
; uint16_t FAT_read(FAT_File far* file, uint16_t byteCount, void* dataOut)
FAT_read:
  ; new call frame
  push bp
  mov bp, sp

  sub sp, 2

  push es
  push bx

  ; [bp - 2] - currentDataOut
  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - file
  ; [bp + 8] - byteCount
  ; [bp + 10] - dataOut

  mov ax, [bp + 10]
  mov [bp - 2], ax

  mov bx, [bp + 4]
  mov es, [bp + 6]
  mov ax, es:[bx + 0] ; handle

  mov es, [fat_open_files + 2]
  mov bx, [fat_open_files]
  mov cx, FILE_DATA_SIZE
  mul word cx
  add bx, ax
  ; es:bx is pointer to file data

  mov ax, es:[bx + 8]
  mov dx, es:[bx + 10] ; size
  mov cx, es:[bx + 6] ; pos hi
  sub dx, cx
  mov cx, es:[bx + 4] ; pos lo
  sbb ax, cx

  mov cx, [bp + 8]

  ; dx:ax contains remaining bytes in file
  or dx, dx
  jnz .byteCountFine

  cmp ax, cx
  jge .byteCountFine

  mov cx, ax

.byteCountFine:
  ; cx contains byte count

  or cx, cx
  jz .takeLoopDone

.takeLoop:
  mov ax, es:[bx + 4]
  and ax, (512 - 1) ; pos % sector_size
  mov dx, SECTOR_SIZE
  sub dx, ax ; bytes left in buffer

  mov ax, cx
  cmp ax, dx
  cmovg ax, dx
  ; ax contains bytes to take

  push cx ; to restore
  push ax ; to restore

  push ax ; count
  push es
  add bx, FILE_DATA_OFFSET
  push bx
  sub bx, FILE_DATA_OFFSET
  push ds
  mov ax, [bp - 2]
  push ax
  call memcpy_far
  add sp, 10

  pop ax ; restore

  mov cx, [bp - 2]
  add cx, ax
  mov [bp - 2], cx ; update dataOut

  mov cx, es:[bx + 4]
  add cx, ax
  mov es:[bx + 4], cx ; update position

  pop cx
  sub cx, ax
  or cx, cx
  jz .takeLoopDone

  push cx ; to restore

  push es
  push bx
  call FAT_read_next_sector_to_buffer
  add sp, 4

  pop cx ; restore

  or ax, ax
  jz .takeLoopDone

  jmp .takeLoop

.takeLoopDone:

  mov ax, [bp - 2]
  mov cx, [bp + 10]
  sub ax, cx

  ; return
  pop bx
  pop es

  mov sp, bp
  pop bp
  ret

; bool FAT_read_next_sector_to_buffer(FAT_FileData far*)
FAT_read_next_sector_to_buffer:
  ; new call frame
  push bp
  mov bp, sp

  push fs
  push si
  push es
  push bx

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - file

  mov es, [bp + 6]
  mov bx, [bp + 4]

  mov ax, es:[bp + 19]
  inc ax
  mov es:[bp + 19], ax ; update current sector

  mov si, [fat_data]
  mov fs, [fat_data + 2]
  xor cx, cx
  mov cl, fs:[si + 13] ; sectors per cluster

  cmp ax, cx
  jl .cluster_updated

  xor ax, ax
  mov es:[bp + 19], ax ; reset current sector

  mov ax, es:[bx + 15]
  push ax
  call FAT_next_cluster
  add sp, 2
  mov es:[bx + 15], ax

  cmp ax, 0xFF8
  jge .invalid_cluster

.cluster_updated:
  mov ax, es:[bx + 15] ; current cluster
  push 0
  push ax
  call FAT_cluster_to_lba
  add sp, 4
  ; dx:ax = lba

  push es
  add bx, 23
  push bx
  push 1
  push dx
  push ax
  mov ax, [disk]
  push ax
  call diskReadSectors
  add sp, 12

  or ax, ax
  jz .read_failed

  mov ax, 1

.finish:
  ; return
  pop bx
  pop es
  pop si
  pop fs

  mov sp, bp
  pop bp
  ret

.invalid_cluster:
  push msg_invalid_cluster
  call puts
  add sp, 2

  xor ax, ax
  jmp .finish

.read_failed:
  push msg_read_failed
  call puts
  add sp, 2

  xor ax, ax
  jmp .finish


; global FAT_readDirectory
; bool FAT_readDirectory(FAT_File far* file, FAT_DirectoryEntry* directoryEntry)
FAT_readDirectory:

global FAT_close
; void FAT_close(FAT_File far* file)
FAT_close:
  ; new call frame
  push bp
  mov bp, sp

  push bx
  push es

  ; [bp + 0] - old bp
  ; [bp + 2] - return address
  ; [bp + 4] - file

  mov bx, [bp + 4]
  mov es, [bp + 6]
  mov ax, es:[bx + 0] ; handle

  mov es, [fat_open_files + 2]
  mov bx, [fat_open_files]
  mov cx, FILE_DATA_SIZE
  mul word cx
  add bx, ax
  ; es:bx is pointer to file data

  ; set is open to false
  mov byte es:[bx + 12], 0

  ; return
  pop es
  pop bx

  mov sp, bp
  pop bp
  ret

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

  mov dx, es:[bx + 14]

  add bx, FAT_DATA_FAT_OFFSET
  push es
  push bx
  push cx
  xor ax, ax
  push ax
  push dx
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
  mov cx, es:[bx + 22] ; sectors per fat
  mul word cx
  mov cx, es:[bx + 14]
  add ax, cx
  adc dx, 0
  push dx
  push ax

  ; update data_section_lba with correct values
  mov cx, [data_section_lba]
  add cx, ax
  mov [data_section_lba], cx
  mov cx, [data_section_lba + 2]
  adc cx, dx
  mov [data_section_lba + 2], cx

  mov ax, [disk]
  push ax
  call diskReadSectors
  add sp, 12

  or ax, ax
  jz .finish

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
msg_too_many_open_files: db 'FAT: Failed to open file, too many already open', ENDL, 0
msg_invalid_cluster: db 'FAT: Invalid cluster', ENDL, 0
msg_read_failed: db 'FAT: Read failed', ENDL, 0

; FAT_Data far*
fat_data: dw 0000h, 0050h

section WDATA CLASS=DATA

; DISK*
disk: dw 0

; FAT_FileData[]
; index 0 is always the root directory
fat_open_files: dw 0000h, 0050h

data_section_lba: dd 0
