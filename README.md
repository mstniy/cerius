# cerius
The Cerius operating system

### What can it do?
* It has a memory manager
* It has AHCI and IDE drivers
* It can read MBR and GPT

### What does it need?
* A file system implementation
* An ELF loader
* A kernel
* Everything that will be done by the kernel
  * Process management
  * Inter-process communication
  * A synchronization framework
* A package manager
* More drivers

### How to compile it?
Cerius uses [nasm](http://www.nasm.us) as the assembler and GCC as the compiler.
You'll also need `make`.
To create the image, which can be emulated or written to a real disk, do a `make harddisk.img`

### How to run it?
After you create the image, you can use an emulator (like Bochs, Qemu or Virtualbox).
If you have Qemu installed, you can do a quick `make simqemu`.
You may also write the image on a real disk and boot real hardware from it (`dd` or Rufus should be fine).
To run it on real hardware, you'll also need an AMD64 CPU.

### I have an Intel CPU, can it run Cerius?
If it's a 64-bit processor, almost certainly.
