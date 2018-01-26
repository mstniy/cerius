; Weight: 66 bytes

[bits 16]
; prints the value of DX as hex.
print_hex:
pusha

mov ah, 0x0e
mov al, '0'
int 0x10
mov al, 'x'
int 0x10

mov bl, dh
shr bl, 4
call print_hex_letter

mov bl, dh
call print_hex_letter


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
jae phl_bl_g_10
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