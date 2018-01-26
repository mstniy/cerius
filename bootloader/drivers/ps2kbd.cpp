#include "ps2kbd.h"

const BYTE PS2KBD_PORT = 0x60;
const BYTE PS2KBD_RESEND = 0xFE;

BYTE kbd_send_cmd(BYTE cmd)
{
	while (1)
	{
		PortIO::OutB(PS2KBD_PORT, cmd);
		BYTE res = PortIO::InB(PS2KBD_PORT);
		if (res != PS2KBD_RESEND)
			return res;
	}
}

void kbd_send_data(BYTE data)
{
	PortIO::OutB(PS2KBD_PORT, data);
}

void Keyboard::changeScrollLock(bool newstate)
{
	kbd_send_cmd(0xED);
	kbd_send_data(newstate);
}

void Keyboard::changeNumberLock(bool newstate)
{
	kbd_send_cmd(0xED);
	kbd_send_data(newstate << 1);
}

void Keyboard::changeCapsLock(bool newstate)
{
	kbd_send_cmd(0xED);
	kbd_send_data(newstate << 2);
}

void Keyboard::Init()
{
	kbd_send_cmd(0xF4);
}

BYTE Keyboard::getScanCode()
{
	return PortIO::InB(PS2KBD_PORT);
}
