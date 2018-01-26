[bits 16]

; WORD __stdcall detect_mem(WORD pBuf, WORD max_entries)

; Enumerates chunks of free memory, using SMAP BIOS function.
; Each item of the list consists of two 64-bit fields.
;   Low address field: Base address of the chunk.
;   High address field: Length of the chunk.
; Returns the number of list items.
;   If the number of free chunks returned by the BIOS exceeds max_entries, aborts the enumeration and returns 0.
; This function is expected to overwrite registers.

detect_mem:
  push bp
  mov bp, sp

  sub sp, 10 ; [bp-2] : cl (buffer length as returned by bios) // [bp-4] : smap continuation value, high word // [bp-6] : smap continuation value, low word // [bp-8] : number of free memory entries added to the list so far // [bp-10] : is this the first call to the bios?

  mov word [bp-4], 0
  mov word [bp-6], 0
  mov word [bp-8], 0
  mov word [bp-10], 1

  .detect_mem_smap_loop:
  movzx eax, word [bp-10]
  or eax, dword [bp-6]
  jz .detect_mem_smap_loop_end ; If bios returned 0 as the continuation value in the previous run, stop iterating.
  mov ax, 0
  mov es, ax
  mov di, SMAP_BUFFER
  mov ebx, dword [bp-6]
  mov edx, 0x534D4150
  mov eax, 0xE820
  mov ecx, 24
  int 0x15
  mov word [bp-10], 0

  jc .detect_mem_smap_loop_end
  cmp eax, 0x534D4150
  jne .detect_mem_error

  mov [bp-2], cl
  mov dword [bp-6], ebx

  cmp byte [bp-2], 20
  je .detect_mem_smap_buffer_length_ok
  cmp byte [bp-2], 24
  je .detect_mem_smap_buffer_length_ok
  jmp .detect_mem_error

  .detect_mem_smap_buffer_length_ok:
    cmp dword [SMAP_TYPE], SMAP_FREE_MEM_TYPE
    jne .detect_mem_smap_loop ; Ignore all memory regions except the free ones.

    mov eax, [SMAP_LENGTH]
    or eax, [SMAP_LENGTH+4]
    cmp eax, 0
    je .detect_mem_smap_loop ; Ignore zero-length entries.
    cmp byte [bp-2], 24
    jne .detect_mem_smap_buffer_no_und_bits
    test dword [SMAP_EXT], 1
    jz .detect_mem_smap_loop ; BIOS indicates the entry should be ignored
    test dword [SMAP_EXT], 2
    jnz .detect_mem_smap_loop ; Do not use non-volatile memory
    .detect_mem_smap_buffer_no_und_bits:
      mov ax, [bp-8]
      cmp ax, [bp+6] ; If the number of free memory chunks exceeded max_entries, return an error.
      jae .detect_mem_error
      inc word [bp-8]
      mov si, SMAP_BUFFER
      mov di, [bp+4]
      mov cx, 16
      .detect_mem_smap_copy_buffer_loop:
        cmp cx, 0
        je .detect_mem_smap_copy_buffer_loop_end
        mov al, [si]
        mov [di], al
        inc si
        inc di
        dec cx
        jmp .detect_mem_smap_copy_buffer_loop
      .detect_mem_smap_copy_buffer_loop_end:
        add word [bp+4], 16
        jmp .detect_mem_smap_loop

  .detect_mem_smap_loop_end:
    mov ax, [bp-8]
    leave
    ret 4

  .detect_mem_error:
    mov ax, 0
    leave
    ret 4

SMAP_BUFFER:
  SMAP_BASE:
    dq 0
  SMAP_LENGTH:
    dq 0
  SMAP_TYPE:
    dd 0
  SMAP_EXT:
    dd 0
SMAP_BUFFER_END:

; Constants

SMAP_FREE_MEM_TYPE equ 1