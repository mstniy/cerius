[bits 16]

; From : http://wiki.osdev.org/Setting_Up_Long_Mode

detect_long_mode:
  mov eax, 0x80000000       ; Set the A-register to 0x80000000.
  cpuid                     ; CPU identification.
  cmp eax, 0x80000001       ; Compare the A-register with 0x80000001.
  jb .fail                  ; It is less, there is no long mode.
  mov eax, 0x80000001       ; Set the A-register to 0x80000001.
  cpuid                     ; CPU identification.
  test edx, 1 << 29         ; Test if the LM-bit, which is bit 29, is set in the D-register.
  jz .fail                  ; They aren't, there is no long mode.
   
  mov eax, 1
  ret

  .fail:
    xor eax, eax
    ret
