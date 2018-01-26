ASM_SOURCES = $(wildcard *.asm)
ASM_OBJS = ${ASM_SOURCES:.asm=.o}

nasm = nasm
ld = ld

bootsector.bin : bootsector.asm *.asm
	$(nasm) $< -f bin -o $@ -Wall

$(ASM_OBJS) : %.o : %.asm
	$(nasm) $< -f elf -o $@ -Wall

clear : 
	del bootsector.bin
	del *.o