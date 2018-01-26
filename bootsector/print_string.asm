[bits 16]
; Prints the zero-terminated string at bx to the screen.
print_string:
pusha
mov ah, 0x0e
pstring_loop_cond:
cmp byte [bx], 0
je pstring_loop_end
mov al, [bx]
int 0x10
add bx, 1
jmp pstring_loop_cond
pstring_loop_end:
popa
ret