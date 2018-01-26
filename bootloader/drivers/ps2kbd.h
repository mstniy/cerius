#ifndef PS2KBD_H
#define PS2KBD_H

#include "../common/type.h"
#include "../common/port_io.h"

class Keyboard
{
public:
	static void changeScrollLock(bool newstate);
	static void changeNumberLock(bool newstate);
	static void changeCapsLock(bool newstate);
	static void Init();
	static BYTE getScanCode();
};

#endif
