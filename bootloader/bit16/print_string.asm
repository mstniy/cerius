[bits 16]
; Prints the zero-terminated string at bx to the screen.
print_string:
  pusha
  mov ah, 0x0e
  pstring_loop:
    cmp byte [bx], 0
    je pstring_loop_end
    mov al, [bx]
    int 0x10
    add bx, 1
    jmp pstring_loop
  pstring_loop_end:
    popa
    ret

; Prints the ascii-encoding character at bl to the screen.
print_char:
  pusha
  mov ah, 0x0e
  mov al, bl
  int 0x10
  popa
  ret

; Prints a new line.
print_line:
  push bx
  mov bl, 13 ; '\r'
  call print_char
  mov bl, 10 ; '\n'
  call print_char
  pop bx
  ret