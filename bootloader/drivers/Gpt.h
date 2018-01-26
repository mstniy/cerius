#ifndef BOOTLOADERGPT_H
#define BOOTLOADERGPT_H

#include "Disk.h"
#include "../shared_ptr/unique_ptr.h"
#include "type.h"

class GptPartition
{
public:
	Disk* disk;
	uint64_t type[2];
	uint64_t guid[2];
	uint64_t lbaStart;
    uint64_t byteLen;
    uint64_t attr;
    uint16_t name[36];
private:
	GptPartition();
	GptPartition(Disk* _disk, uint64_t* _type, uint64_t* _guid, uint64_t _lbaStart, uint64_t lastLBA, uint64_t _attr, uint16_t* _name);
public:
	bool Read(uint64_t relLBAStart, uint64_t byteCnt, BYTE* buf);
	bool IsValid() const;

	friend class Gpt;
};

class Gpt
{
public:
	uint64_t diskGuid[2];
	int numPartitions;
	unique_array<GptPartition> parts;
public:
	Gpt();
	bool Init(Disk* disk);
};

#endif
