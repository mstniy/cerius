#ifndef DISPLAY8025_COL_H
#define DISPLAY8025_COL_H

#include <stdint.h>

#include "../common/utils.h"
#include "../common/port_io.h"

void pchar(char character);
void pstring(const char* message);
void pbyte(uint8_t b);
void pword(uint16_t w);
void pdword(uint32_t d);
void pqword(uint64_t q);
void pptr(const void* p);
void pbool(bool b);
void pdecimal(uint64_t q);
void clear_screen();

#endif
