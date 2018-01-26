bochs = bochs
qemu = qemu-system-x86_64
qemu-img = qemu-img
make = make
gpp = g++ --std=c++11

bootsector/bootsector.bin : FORCE
	$(make) -C bootsector bootsector.bin

bootloader/bootloader.bin : FORCE
	$(make) -C bootloader bootloader.bin
	
image.bin : bootsector/bootsector.bin bootloader/bootloader.bin
	copy bootsector\bootsector.bin /b + bootloader\bootloader.bin /b $@ /b

# On linux, 'truncate' can be used instead
extend_file.exe : extend_file.cpp
	$(gpp) $< -o $@
	
harddisk.img : image.bin extend_file.exe
	copy $< $@
	extend_file.exe $@ 10321920

sim : harddisk.img cerius.bxrc
	$(bochs) -f cerius.bxrc -q

simqemu: harddisk.img
	$(qemu) -drive file=harddisk.img,format=raw -m 512M
	
simqemuahci: harddisk.img
	$(qemu) -device ahci,id=ahci0 -drive if=none,file=harddisk.img,format=raw,id=drive -device ide-drive,bus=ahci0.0,drive=drive,id=sata0-0-0 -m 512M
	
harddisk.vmdk: harddisk.img
	$(qemu-img) convert -O vmdk $< $@
	
clear : FORCE
	$(make) -C bootsector clear
	$(make) -C bootloader clear
	del image.bin
	del harddisk.img
	del harddisk.vmdk
	del extend_file.exe
	
FORCE: