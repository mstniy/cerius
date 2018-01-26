[BITS 16]

detect_capabilities:
  call detect_cpuid
  test eax, eax
  jz .fail

  call detect_long_mode
  test eax, eax
  jz .fail

  call detect_sse2
  test eax, eax
  jz .fail

  mov eax, 1
  ret

  .fail:
  xor eax, eax
  ret

%include "bit16/detect_cpuid.asm"
%include "bit16/detect_long_mode.asm"
%include "bit16/detect_sse2.asm"