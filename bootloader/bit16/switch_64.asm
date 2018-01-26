[bits 16]

; DO NOT CALL THIS FUNCTION, JMP TO IT!
; Enables IA-32e paging, switches to long mode.
; Once the switch is done, jmp's to the label named 'long_mode_begin'

; NORETURN switch_64();

switch_64:
  cli
  push bp
  mov bp, sp
  ; Load the GDTR and the IDTR
  lgdt [gdt64_descriptor]
  lidt [idt64_descriptor]
  ; Enable protection
  mov eax, cr0
  or eax, 1
  mov cr0, eax
  ; Enable PAE
  mov eax, cr4
  or eax, 1<<5
  mov cr4, eax
  ; Set the LM bit
  mov ecx, 0xC0000080
  rdmsr
  or eax, 1<<8
  wrmsr
  ; Enable paging
  mov eax, paging64.pml4
  mov cr3, eax
  mov eax, cr0
  or eax, 1<<31 ; PG
  mov cr0, eax
  ; Fiddle with control registers
  mov eax, cr0
  or eax, 1<<16 ; WP
  mov cr0, eax

  ;mov eax, cr4
  ;or eax, 1<<11 ; UMIP ; TODO: Enable these only if the CPU supports them.
  ;or eax, 1<<17 ; PCIDE
  ;or eax, 1<<20 ; SMEP
  ;mov cr4, eax

  mov ecx, 0xC0000080 ; IA32_EFER
  rdmsr
  or eax, 1<<11 ; NXE
  wrmsr

  ;Enable SSE, because GCC likes to use it.
  mov eax, cr0
  and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
  or ax, 0x2			;set coprocessor monitoring  CR0.MP
  mov cr0, eax
  mov eax, cr4
  or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
  mov cr4, eax

  leave ; Restore the stack frame before switching to 64-bit mode
  ; Switch to long mode by a long jmp
  jmp 8:.long_mode_dummy_label
  .long_mode_dummy_label:
  ; Now we're in long mode
[BITS 64]
    mov ax, 16 ; Data segment
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    jmp long_mode_begin


gdt64_start:
  ; Null descriptor
  dd 0
  dd 0
  ; Code segment
  dd 0
  db 0
  db 10011010b
  db 00100000b
  db 0
  ; Data segment
  dd 0
  db 0
  db 10010010b
  db 0
  db 0
  ; To-be TSS
  dq 0
  dq 0

gdt64_descriptor:
  dw 39
  dq gdt64_start

idt64_descriptor:
  dw 0
  dq 0
