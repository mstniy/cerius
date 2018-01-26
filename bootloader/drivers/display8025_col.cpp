#include "display8025_col.h"

uint8_t* const VIDEO_ADDRESS = (uint8_t*)0xb8000;
const int MAX_ROWS = 25;
const int MAX_COLS = 80;
// Attribute byte for our default colour scheme.
const uint8_t WHITE_ON_BLACK = 0x0f;
// Screen device I/O ports
const uint16_t REG_SCREEN_CTRL = 0x3D4;
const uint16_t REG_SCREEN_DATA = 0x3D5;
int d8025_offset;
int d8025_offset_initialized = 0;

int get_screen_offset(int col, int row)
{
	return (row*MAX_COLS + col)*2;
}
int get_cursor()
{
	if (d8025_offset_initialized)
		return d8025_offset;
	// The device uses its control register as an index
	// to select its internal registers , of which we are
	// interested in:
	// reg 14: which is the high byte of the cursor’s offset
	// reg 15: which is the low byte of the cursor’s offset
	// Once the internal register has been selected , we may read or
	// write a byte on the data register.
	PortIO::OutB(REG_SCREEN_CTRL , 14);
	int offset = PortIO::InB(REG_SCREEN_DATA) << 8;
	PortIO::OutB(REG_SCREEN_CTRL , 15);
	offset += PortIO::InB(REG_SCREEN_DATA);
	// Since the cursor offset reported by the VGA hardware is the
	// number of characters , we multiply by two to convert it to
	// a character cell offset.
	offset *= 2;
	d8025_offset = offset;
	d8025_offset_initialized = 1;
	return d8025_offset;
}
void set_cursor(int offset)
{
	d8025_offset = offset;
	d8025_offset_initialized = 1;
	offset /= 2; // Convert from cell offset to char offset.
	// This is similar to get_cursor , only now we write
	// bytes to those internal device registers
	PortIO::OutB(REG_SCREEN_CTRL , 14);
	PortIO::OutB(REG_SCREEN_DATA , (unsigned char)(offset >> 8));
	PortIO::OutB(REG_SCREEN_CTRL , 15);
	PortIO::OutB(REG_SCREEN_DATA , (unsigned char)(offset & 0xff));
}
int get_row_from_offset(int offset)
{
	return (offset/2)/MAX_COLS;
}
int get_col_from_offset(int offset)
{
	return (offset/2)%MAX_COLS;
}
// Shifts the contents on the screen *x* many rows upwards, clears the bottom rows.
void scroll_up(int x)
{
	if (x==0)
		return ;
	for (int row=0; row+x < MAX_ROWS; row++)
	{
		memory_copy(VIDEO_ADDRESS + get_screen_offset(0, row),
			    VIDEO_ADDRESS + get_screen_offset(0, row+x),
			    MAX_COLS*2);
	}
	for (int curRow = MAX_ROWS-x; curRow<MAX_ROWS; curRow++)
		for (int curCol=0; curCol < MAX_COLS; curCol++)
			*(uint8_t*)(VIDEO_ADDRESS+get_screen_offset(curCol, curRow))=' ';
}
/* Print a char on the screen at cursor position */
void pchar(char character)
{
	/* Get the video memory offset for the screen location */
	int offset = get_cursor();

	if (character == '\n')
	{
		int row = get_row_from_offset(offset);
		if (row == MAX_ROWS-1)
		{
			scroll_up(1);
			row--;
		}
		set_cursor(get_screen_offset(0, row+1));
	}
	else
	{
		VIDEO_ADDRESS[offset] = character;
		VIDEO_ADDRESS[offset+1] = WHITE_ON_BLACK;
		if ((get_row_from_offset(offset) == MAX_ROWS-1) &&
		    (get_col_from_offset(offset) == MAX_COLS-1))
		{
			scroll_up(1);
			offset -= 2*MAX_COLS;
		}
		set_cursor(offset+2);
	}
}
void pstring(const char* message)
{
	/*for (int i=0;message[i];i++)
		pchar(message[i]);*/
	int numCharAtRow[MAX_ROWS]={0};
    int tmpRow = get_row_from_offset(get_cursor());
    int tmpCol = get_col_from_offset(get_cursor());
    int scrollUpCnt=0;
    int startIndex=0;
    for (int i=0; message[i] != '\0' ; i++, startIndex++)
	{
		numCharAtRow[tmpRow]++;
		if (message[i] == '\n')
		{
			tmpRow++;
			tmpCol=0;
		}
		else
			tmpCol++;
        if (tmpCol==MAX_COLS)
		{
			tmpRow++;
			tmpCol=0;
		}
		if (tmpRow==MAX_ROWS)
		{
			tmpRow=MAX_ROWS-1;
			scrollUpCnt++;
			for (int i=0;i<MAX_ROWS-1;i++)
				numCharAtRow[i]=numCharAtRow[i+1];
			numCharAtRow[MAX_ROWS-1]=0;
		}
	}
	for (int i=0;i<MAX_ROWS;i++)
		startIndex -= numCharAtRow[i];
    if (scrollUpCnt > MAX_ROWS)
        scrollUpCnt = MAX_ROWS;
	scroll_up(scrollUpCnt);
	int tmpCursor = get_cursor()-scrollUpCnt*2*MAX_COLS;
	if (tmpCursor<0)
		tmpCursor=0;
    for (int i=startIndex; message[i]!='\0' ;i++)
    {
    	if (message[i] != '\n')
		{
    		VIDEO_ADDRESS[tmpCursor]=message[i];
    		VIDEO_ADDRESS[tmpCursor+1]=WHITE_ON_BLACK;
    		tmpCursor+=2;
		}
		else
            tmpCursor += (2*MAX_COLS)-tmpCursor%(2*MAX_COLS);
    }
    set_cursor(tmpCursor);
}
void pbool(bool b)
{
	pstring(b ? "true":"false");
}
void _pbyte(uint8_t b)
{
	for (int shr=4; shr >= 0 ; shr-=4)
	{
		uint8_t bs = b >> shr;
		bs &= 0x0F;
		if (bs < 10)
			pchar(bs + '0');
		else
			pchar(bs + 'A' - 10);
	}
}
void _pword(uint16_t w)
{
	_pbyte(w>>8);
	_pbyte(w);
}
void _pdword(uint32_t d)
{
	_pword(d>>16);
	_pword(d);
}
void _pqword(uint64_t q)
{
	_pdword(q>>32);
	_pdword(q);
}
void pbyte(uint8_t b)
{
	pstring("0x");
	_pbyte(b);
}
void pword(uint16_t w)
{
	pstring("0x");
	_pword(w);
}
void pdword(uint32_t d)
{
	pstring("0x");
	_pdword(d);
}
void pqword(uint64_t q)
{
	pstring("0x");
	_pqword(q);
}
void pptr(const void* p)
{
	pqword((uint64_t)p);
}
void pdecimal(uint64_t q)
{
	char buf[20]={0};
	int bufpos=sizeof(buf)/sizeof(char)-2; // Reserve one field for the null terminator.
	if (q == 0)
		buf[bufpos--]='0';
    while (q != 0)
	{
        buf[bufpos--]='0'+q%10;
		q/=10;
	}
	pstring(buf+bufpos+1);
}
void clear_screen()
{
	int row = 0; int col = 0;
	/* Loop through video memory and write blank characters. */
	for (row=0; row<MAX_ROWS; row++)
		for (col=0; col<MAX_COLS; col++)
			*(uint8_t*)(VIDEO_ADDRESS+get_screen_offset(col, row))=' ';

	// Move the cursor back to the top left.
	set_cursor(get_screen_offset(0, 0));
}
