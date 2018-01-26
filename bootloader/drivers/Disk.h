#ifndef DISK_H
#define DISK_H

#include <stdint.h>

class Disk
{
public:
	virtual bool Read(uint64_t lbaStart, uint64_t byteCnt, uint8_t* buf) = 0;
	virtual uint64_t GetCapacity() = 0; // In bytes
};

#endif
