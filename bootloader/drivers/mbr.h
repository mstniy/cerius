#ifndef MBR_H
#define MBR_H

#include "Disk.h"
#include "common/type.h"

class MbrPartition
{
public:
	Disk* disk;
	BYTE status;
	BYTE partition_type;
	uint64_t lbaStart;
	uint64_t byteLen;
public:
	bool Read(uint64_t relLBAStart, uint64_t byteCnt, BYTE* buf);
	bool IsValid() const;
};

class Mbr
{
public:
	MbrPartition partitions[4];
public:
	bool Init(Disk* disk);
};

#endif
