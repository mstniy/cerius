[bits 16]

; uint64_t adjust_page_table(uint32_t initReserveLower, uint32_t initReserveHigher);

; Identity maps the first megabyte and the given initial reserve with kernel-exclusive read-write access.
; Note that initReserveLower/Higher are physical addresses which contain 2mb of usable memory.
; Sets eax to the lower half of the linear address of the 2mb-long initial reserve
; Sets ebx to the higher half

adjust_page_table:
  push bp
  mov bp, sp
  ; Initialize the first entry of PML4
  mov ebx, paging64.pml4
  mov dword [ebx], paging64.pdpt
  or  dword [ebx], 3
  mov dword [ebx+4096], paging64.pdpt
  ; Initialize the first entry of PDPT
  mov ebx, paging64.pdpt
  mov dword [ebx], paging64.pd
  or  dword [ebx], 3
  mov dword [ebx+4096], paging64.pd
  ; Initialize the first entry of PD
  mov ebx, paging64.pd
  mov dword [ebx], paging64.pt
  or  dword [ebx], 3
  mov dword [ebx+4096], paging64.pt
  ; Initialize the relevant part of PT
  ; eax : current physical address to map, ebx: current physical address of current PT entry, ecx: number of PT entries initialized so far
  mov eax, 0
  mov ecx, 0
  mov ebx, paging64.pt
  .pt_loop_begin:
    cmp ecx, 256 ; 1 mb = 256 * 4kb
    jae .pt_loop_end
    mov edx, eax
    or edx, 11b
    mov [ebx], edx
    ;mov [ebx+4096], edx ; We don't need to set the linear fields for PT entries, because they don't have any child.
    add eax, 4096 ; 4kb
    add ebx, 8
    inc ecx
    jmp .pt_loop_begin
  .pt_loop_end:
  mov eax, [bp+4] ; Page in the initial reserve
  mov ebx, [bp+8]
  or eax, 10000011b ; Present, writable, pagesize
  mov [paging64.pd+8], eax
  mov [paging64.pd+12], ebx ; No need to initialize the lin field of the entry because it maps a page, because it has pagesize bit set.
  mov eax, 2*1024*1024 ; TODO: Randomize the linear address of the initial reserve.
  xor ebx, ebx
  leave
  ret 8

paging64:
  times 4096-(($-$$+0x7E00) % 4096) db 0 ; 4kb alignment
  .pml4:
    times 512 dq 0 ; This is the format expected by 64bit Pager
    times 512 dq 0
  .pdpt:
    times 512 dq 0
    times 512 dq 0
  .pd:
    times 512 dq 0
    times 512 dq 0
  .pt:
    times 512 dq 0
    times 512 dq 0