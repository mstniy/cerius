#ifndef PORT_IO_H
#define PORT_IO_H

#include "type.h"

class PortIO
{
public:
	PortIO() = delete;
	~PortIO() = delete;

	static BYTE InB(WORD port);
	static void OutB(WORD port , BYTE data);
	static WORD InW(WORD port);
	static void OutW(WORD port , WORD data);
	static DWORD InD(WORD port);
	static void OutD(WORD port , DWORD data);
};

#endif
