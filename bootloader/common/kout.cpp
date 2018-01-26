#include "kout.h"

KPrinter kout;

KPrinter& KPrinter::operator<<(unsigned int v)
{
	(*this)<<(uint64_t)v;
	return (*this);
}

KPrinter& KPrinter::operator<<(int v)
{
	(*this) << (int64_t)v;
	return (*this);
}
KPrinter& KPrinter::operator<<(bool v)
{
	pbool(v);return (*this);
}
KPrinter& KPrinter::operator<<(const void* v)
{
	pptr(v);return (*this);
}
KPrinter& KPrinter::operator<<(int64_t v)
{
	if (v<0)
	{
		pchar('-');
		v=-v;
	}
	pdecimal((uint64_t)v);return (*this);
}
KPrinter& KPrinter::operator<<(uint64_t v)
{
	pdecimal(v);return (*this);
}
KPrinter& KPrinter::operator<<(char v)
{
	pchar(v);return (*this);
}
KPrinter& KPrinter::operator<<(const char* v)
{
	pstring(v);return (*this);
}
