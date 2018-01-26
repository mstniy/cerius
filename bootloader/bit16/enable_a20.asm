[bits 16]

; From: https://wiki.osdev.org/A20_Line

enable_a20:
  cli
 
  call    .a20wait_out
  mov     al,0xAD ; Disable first PS/2 port
  out     0x64,al
 
  call    .a20wait_out
  mov     al,0xD0 ; Read controller output port
  out     0x64,al
 
  call    .a20wait_in
  in      al,0x60
  push    ax
 
  call    .a20wait_out
  mov     al,0xD1 ; Write to controller output port
  out     0x64,al
 
  call    .a20wait_out
  pop     ax
  or      al,2    ; Set A20 gate output
  out     0x60,al
 
  call    .a20wait_out
  mov     al,0xAE ; Enable first PS/2 port
  out     0x64,al
 
  call    .a20wait_out
  ret
 
  .a20wait_out: ; Waits for the input buffer to become empty
    in      al,0x64
    test    al,2
    jnz     .a20wait_out
    ret
  
  .a20wait_in: ; Waits for the output buffer to become empty
    in      al,0x64
    test    al,1
    jz      .a20wait_in
    ret
