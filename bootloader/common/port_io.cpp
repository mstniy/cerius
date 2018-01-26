#include "port_io.h"

BYTE PortIO::InB(WORD port)
{
	// A handy C wrapper function that reads a byte from the specified port
	// "=a" (result) means: put AL register in variable RESULT when finished
	// "d" (port) means: load EDX with port
	BYTE result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}
void PortIO::OutB(WORD port , BYTE data)
{
	// "a" (data) means: load EAX with data
	// "d" (port) means: load EDX with port
	__asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}
WORD PortIO::InW(WORD port)
{
	WORD result;
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}
void PortIO::OutW(WORD port , WORD data)
{
	__asm__("out %%ax, %%dx" : :"a" (data), "d" (port));
}
DWORD PortIO::InD(WORD port)
{
	DWORD result;
	__asm__("in %%dx, %%eax" : "=a" (result) : "d" (port));
	return result;
}
void PortIO::OutD(WORD port , DWORD data)
{
	__asm__("out %%eax, %%dx" : :"a" (data), "d" (port));
}
