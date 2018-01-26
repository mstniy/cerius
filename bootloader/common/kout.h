#ifndef KOUT_H
#define KOUT_H

#include<stdint.h>
#include "drivers/display8025_col.h"

class KPrinter
{
public:
	KPrinter& operator<<(unsigned int v);
    KPrinter& operator<<(int v);
    KPrinter& operator<<(bool v);
    KPrinter& operator<<(const void* v);
    KPrinter& operator<<(int64_t v);
    KPrinter& operator<<(uint64_t v);
    KPrinter& operator<<(char v);
    KPrinter& operator<<(const char* v);
};

extern KPrinter kout;

#endif
