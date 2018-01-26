#include "../drivers/display8025_col.h"

extern "C" void __cxa_pure_virtual() // GCC invokes this function when a pure virtual method is called
{
	pstring("Pure virtual method called. Hanging...");
	asm volatile("cli\n\thlt");
}
