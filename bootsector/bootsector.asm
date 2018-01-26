[bits 16]
[org 0x7C00] ; BIOS loads the bootsector to address 0x7C00

xor ax, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

mov [BOOT_DRIVE], dl

mov bp, 0x7C00 ; Set the stack below the bootsector code, because the bootloader will be loaded above the bootsector code.
mov sp, bp

mov bx, BOOTLOADER_LOADING_MSG
call print_string
push BOOTLOADER_OFFSET
push 0 ; Start from segment 0.
push BOOTLOADER_SECTOR_SIZE
push 1
push word [BOOT_DRIVE]
call disk_load
cmp ax, 1
jne boot_sector_load_bootloader_failed
jmp boot_sector_load_bootloader_done

boot_sector_load_bootloader_failed:
mov bx, BOOTLOADER_LOAD_FAILED_MSG
call print_string
jmp hang

boot_sector_load_bootloader_done:
mov bx, BOOTLOADER_LOADED_MSG
call print_string
jmp BOOTLOADER_OFFSET

hang:
cli
hlt
jmp hang


; Global variables
BOOT_DRIVE: db 0
BOOTLOADER_LOADING_MSG: db 'Load bootldr mem.', 0
BOOTLOADER_LOAD_FAILED_MSG: db 'Fail load bootldr.', 0
BOOTLOADER_LOADED_MSG: db 'Jmp bootldr.', 0
BOOTLOADER_OFFSET equ 0x7C00+512 ; 0x7E00
BOOTLOADER_SECTOR_SIZE equ 512

%include "disk_load.asm"
%include "print_string.asm"
%include "utils.asm"
%include "print_hex.asm"

; Padding and magic BIOS number.
times 510-($-$$) db 0
dw 0xaa55