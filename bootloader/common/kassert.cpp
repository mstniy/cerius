#include "assert.h"
#include "../drivers/display8025_col.h"
#include "common/utils.h"

void kassert_fail(const char* exp, const char* file, unsigned line)
{
	asm volatile("cli");
	pstring("Assertion failed!\n\nFile: ");
	pstring(file);
    pstring(", Line ");
    pdecimal(line);
    pstring("\n\nExpression: ");
    pstring(exp);
    pstring("\n\nHalting...");
}
