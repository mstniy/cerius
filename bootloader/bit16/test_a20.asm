[bits 16]

; From : http://www.independent-software.com/writing-your-own-toy-operating-system-enabling-the-a20-line/
; Returns in AX 1 if A20 is enabled, 0 otherwise.

test_a20:
  pushf                          ; Save registers that
  push ds                        ; we are going to
  push es                        ; overwrite.

  cli                            ; No interrupts, please

  xor ax, ax                     ; Set es:di = 0000:0500
  mov es, ax
  mov di, 0x0500

  mov ax, 0xffff                 ; Set ds:si = ffff:0510
  mov ds, ax
  mov si, 0x0510
 
  mov ecx, 8
  .loop:
  cmpsd
  jne .enabled               ; If we come accross with two dword's exactly 1mb apart that have different values, A20 line must be enabled.
  loop .loop

  mov di, 0x0500             ; Otherwise, we need to write to memory to be sure. It might be just a coincidence that these dword's have exactly the same values.
  mov si, 0x0510

  mov al, byte [es:di]       ; Save byte at es:di on stack.
  push ax                        ; (we want to restore it later)

  mov al, byte [ds:si]       ; Save byte at ds:si on stack.
  push ax                        ; (we want to restore it later)

  mov byte [es:di], 0x00     ; [es:di] = 0x00
  mov byte [ds:si], 0xFF     ; [ds:si] = 0xff

  cmp byte [es:di], 0xFF     ; Did memory wrap around?

  pop ax
  mov byte [ds:si], al       ; Restore byte at ds:si

  pop ax
  mov byte [es:di], al       ; Restore byte at es:di

  mov ax, 0
  je .exit             ; If memory wrapped around, return 0.
  .enabled:
  mov ax, 1                      ; else return 1.
 .exit:
  pop es                         ; Restore saved registers.
  pop ds
  popf
  ret