[bits 64]
[extern bootloader_64bit_stage]

; This "jmp" acts as a trampoline. The 16-bit stage jumps to this instruction and this instruction jumps to the C++ function
jmp bootloader_64bit_stage