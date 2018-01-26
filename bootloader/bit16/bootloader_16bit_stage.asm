[bits 16]
[org 0x7E00] ; Bootsector jmp's to 0x7E00 after it loads the bootloader into the memory

mov sp, 0x7C00 ; Addresses 0x7C00 through 0x7E00 is reserved for the SMAP. Stack starts at 0x7C00 and grows downwards.
mov bp,sp

sub sp, 10 ;  word [bp-2] : Number of SMAP entries as returned by detect_mem function.
           ; dword [bp-6] : Initial reserve linear address, as mapped by adjust_page_table (higher bits)
           ; dword [bp-10] : Initial reserve linear address, as mapped by adjust_page_table (lower bits)

mov bx, TESTING_A20_MSG
call print_string
call test_a20
test ax, ax
jnz a20_enabled
mov bx, ENABLING_A20_MSG
call print_string
call enable_a20
mov bx, TESTING_A20_MSG
call print_string
call test_a20
test ax, ax
jz a20_cannot_enable
a20_enabled:
mov bx, A20_ENABLED_MSG
call print_string
mov bx, DETECTING_CAPABILITIES_MSG
call print_string
call detect_capabilities
test eax, eax
jz missing_capabilities
mov bx, DETECTING_MEMORY_MSG
call print_string

push 32 ; We have 512 bytes reserved for the SMAP. Since each entry is 16 bytes long, that gives us a maximum of 32 entries.
push 0x7C00;
call detect_mem
cmp ax, 0
je smap_failed
mov word [bp-2], ax
push ax
push 0x7C00
call find_place_for_initial_reserve
mov ecx, eax
or ecx, ebx
test ecx, ecx
jz no_initial_reserve_found
push ebx ; We need not remember the physical address of the initial reserve
push eax
call adjust_page_table
mov dword [bp-6], ebx
mov dword [bp-10], eax
jmp switch_64
[bits 64]
long_mode_begin:
mov rdi, paging64.pml4
mov rsi, 0x7C00
movzx rdx, word [rbp-2]
mov rcx, qword [rbp-10]
mov r8, 2*1024*1024
add sp, 10
call STAGE64_BEGIN ; bootloader_64bit_stage(paging64.pml4, smap, smap_length, initialreservestart, initialreservelength)
cli ; If the kernel returns, shut the cpu down.
hlt


[BITS 16]

a20_cannot_enable:
  mov bx, A20_CANNOT_ENABLE_MSG
  call print_string
  jmp hang

missing_capabilities:
  mov bx, MISSING_CAPABILITIES_MSG
  call print_string
  jmp hang

smap_failed:
  mov bx, SMAP_FAILED_MSG
  call print_string
  jmp hang

no_initial_reserve_found:
  mov bx, NO_INITIAL_RESERVE_MSG
  call print_string
  jmp hang

hang:
cli
hlt
jmp hang


; Constants
TESTING_A20_MSG: db 'Testing if A20 line is enabled...', 0
ENABLING_A20_MSG: db 'Enabling A20 line...', 0
A20_ENABLED_MSG: db 'A20 line is enabled.', 0
A20_CANNOT_ENABLE_MSG: db 'Cannot enable A20 line. Hanging...', 0
DETECTING_CAPABILITIES_MSG: db 'Checking CPU capabilities...', 0
DETECTING_MEMORY_MSG: db 'Detecting memory...', 0
SMAP_FAILED_MSG: db 'Smap failed. Hanging...', 0
MISSING_CAPABILITIES_MSG: db 'No support for CPUID, long mode or SSE2. Hanging...', 0
NO_INITIAL_RESERVE_MSG: db 'No suitable range for the initial memory reserve was found. Hanging...', 0

; Constants
STAGE64_BEGIN equ 0x11E00 ; Linking 16-bit and 64-bit code together seems to confuse the linker. To avoid linking the 16-bit stage with the rest of the bootloader, we jump to a specific address in memory once we switch from 16-bit mode to 64-bit mode. We also tell the linker to put a "jmp" instruction to that address, which jumps to the actual entry point of the 64-bit stage.


%include "bit16/print_string.asm" ; NASM is weird and searches for these includes in its cd, instead of the directory of the code.
%include "bit16/test_a20.asm"
%include "bit16/enable_a20.asm"
%include "bit16/print_hex.asm"
%include "bit16/detect_capabilities.asm"
%include "bit16/detect_mem.asm"
%include "bit16/initial_reserve.asm"
%include "bit16/switch_64.asm"
%include "bit16/adjust_page_table.asm"

times (STAGE64_BEGIN-0x7E00)-($-$$) db 0