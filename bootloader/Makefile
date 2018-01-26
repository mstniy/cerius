# TODO: Can we get 'make' to search these directories recursively for C++ files?
MEMORY_SRC_DIRS = $(wildcard memory/*)
SRC_DIRS = drivers drivers/Ata common interrupt tests $(MEMORY_SRC_DIRS)
C_SOURCES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
C_OBJS = ${C_SOURCES:.cpp=.o}
ASM_DIRS = asm interrupt
ASM_SOURCES = $(foreach dir,$(ASM_DIRS),$(wildcard $(dir)/*.asm))
ASM_OBJS = ${ASM_SOURCES:.asm=.o}

nasm = nasm
# -fno-rtti : Disables runtime type identification, i.e., dynamic_cast
# -fno-exceptions: Disables exceptions and all the related crazyness.
gpp = g++ -fno-rtti -fno-exceptions -nostdlib -ffreestanding -fno-use-cxa-atexit --std=c++11 -I. -Icommon
ld = ld

bit16/bootloader_16bit_stage.bin : bit16/bootloader_16bit_stage.asm bit16/*.asm
	$(nasm) $< -f bin -o $@ -Wall
	
bootloader_64bit_stage.o : bootloader_64bit_stage.cpp
	$(gpp) -c $< -o $@
	
bootloader_64bit_stage_caller.elf : bootloader_64bit_stage_caller.asm
	$(nasm) $< -f elf64 -o $@ -Wall

bootloader_64bit_stage_with_caller.pe : mylinkscript.txt bootloader_64bit_stage.o bootloader_64bit_stage_caller.elf $(C_OBJS) $(ASM_OBJS)
	$(ld) -T $< -o $@ $(filter-out $<,$^)
	
bootloader_64bit_stage_with_caller.bin : bootloader_64bit_stage_with_caller.pe
	objcopy -O binary $< $@
	
bootloader.bin : bit16/bootloader_16bit_stage.bin bootloader_64bit_stage_with_caller.bin
	copy bit16\bootloader_16bit_stage.bin /b + bootloader_64bit_stage_with_caller.bin /b $@ /b

$(C_OBJS) : %.o : %.cpp
	$(gpp) -c $< -o $@
	
$(ASM_OBJS) : %.o : %.asm
	$(nasm) $< -f elf64 -o $@ -Wall
	
clear : 
	$(foreach dir,$(SRC_DIRS), $(shell del "$(dir)\*.o"))
	del bit16\*.bin
	del asm\*.o
	del *.bin
	del *.elf
	del *.pe
	del *.o
	