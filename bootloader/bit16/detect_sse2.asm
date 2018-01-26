[BITS 16]

detect_sse2:
  mov eax, 1
  cpuid
  and edx, 1<<26
  test edx, edx
  jz .fail

  mov eax, 1
  ret

  .fail:
  xor eax, eax
  ret