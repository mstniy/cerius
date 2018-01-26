[BITS 16]

; QWORD find_place_for_initial_reserve(SmapEntry* smap_start, int16_t smap_lenth);
;
; Iterates the given smap, finds a suitable location for the initial reserve, returns its address. If no suitable range was found, returns (QWORD)0
; (Returns higher dword in ebx, lower dword in eax)
; Suitable: A contiguous range of physical memory that doesn't overlap with the first megabyte, is 2mb-aligned and is at least 2mb long is suitable.

find_place_for_initial_reserve:
  push bp
  mov bp, sp
  sub sp, 2 ; word [bp-2]: curRangeIndex
  mov word [bp-2], 0
  .loop_start:
    mov ax, word [bp-2]
    cmp ax, word [bp+6]
    jge .loop_end
    ; eax: curRangeStart (low), ebx: curRangeStart (high), ecx: curRangeEnd (low), edx: curRangeEnd(high)
    mov ax, word [bp+4] ; curRangeStart = smap_start[curRangeIndex].base
    mov bx, word [bp-2]
    shl bx, 4
    add bx, ax
    mov ecx, dword [bx+8]
    mov edx, dword [bx+12]
    mov eax, dword [bx]
    mov ebx, dword [bx+4]
    add ecx, eax
    adc edx, ebx
    cmp ebx, 0
    jne .start_high_enough
    cmp eax, 1*1024*1024 ; 1MB
    jge .start_high_enough
    mov eax, 1*1024*1024 ; If curRangeStart<oneMB, set it to oneMB
    .start_high_enough:
    test eax, 2*1024*1024-1 ; The base address of the initial reserve must be 2mb aligned, since we'll use the pagesize trick while adding to the page tree.
    jz .start_well_aligned
    and eax, ~(2*1024*1024-1)
    add eax, 2*1024*1024
    adc ebx, 0
    .start_well_aligned:
    mov esi, ecx ; This comparison could be made using fewer registers:
    mov edi, edx ;  Compare high bits of start and end, if they're not equal, that's the result of comparison.
    sub esi, eax ;  If they're equal, compare lower bits. That's the result of the comparison.
    sbb edi, ebx
    jc .loop_continue ; curRangeEnd<curRangeStart
    cmp edi, 0
    jne .loop_suitable
    cmp esi, 2*1024*1024
    jge .loop_suitable
    .loop_continue:
    inc word [bp-2]
    jmp .loop_start
    .loop_suitable:
    leave ; eax and ebx already contain return values
    ret 4
  .loop_end:
    mov eax, 0
    mov ebx, 0
    leave
    ret 4
