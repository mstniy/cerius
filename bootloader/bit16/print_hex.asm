[bits 16]
; prints the value of DX as hex.
print_word:
  pusha

  ;mov ah, 0x0e
  ;mov al, '0'
  ;int 0x10
  ;mov al, 'x'
  ;int 0x10

  mov cx, dx
  mov dl, dh
  call print_byte
  mov dl, cl
  call print_byte

  popa
  ret

; Print the value of DL as hex.
print_byte:
  pusha

  mov bl, dl
  shr bl, 4
  call print_hex_letter
  mov bl, dl
  call print_hex_letter

  popa
  ret

; print the value of BLL as a single hex letter
print_hex_letter:
  pusha
  mov ah, 0x0e

  and bl, 0xf
  cmp bl, 10
  jge phl_bl_g_10
    mov al, '0'
    add al, bl
    jmp phl_bl_cmp_end
  phl_bl_g_10:
    mov al, 'A'
    add al, bl
    sub al, 10
  phl_bl_cmp_end:
  int 0x10

  popa
  ret

; Prints the content of edx as hex.

print_dword:
  pusha
  pushad

  mov cx, dx
  shr edx, 16
  call print_word
  mov dx, cx
  call print_word

  popad
  popa
  ret

; Prints the content of eax:edx as hex.

print_qword:
  pusha
  pushad

  mov ecx, edx
  mov edx, eax
  call print_dword
  mov edx, ecx
  call print_dword

  popad  
  popa
  ret