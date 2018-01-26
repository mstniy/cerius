#include "Gpt.h"

#include "kassert.h"

GptPartition::GptPartition():
	disk(nullptr),
	lbaStart(0),
	byteLen(0)
{
}

GptPartition::GptPartition(Disk* _disk, uint64_t* _type, uint64_t* _guid, uint64_t _lbaStart, uint64_t lastLBA, uint64_t _attr, uint16_t* _name)
{
	disk = _disk;
	type[0] = _type[0];
	type[1] = _type[1];
	guid[0] = _guid[0];
	guid[1] = _guid[1];
	lbaStart = _lbaStart;
	byteLen = 512*(lastLBA-lbaStart+1); // lastLBA is inclusive
	attr = _attr;
	memory_copy((BYTE*)name, (BYTE*)_name, 72);
}

bool GptPartition::Read(uint64_t relLBAStart, uint64_t byteCnt, BYTE* buf)
{
	kassert((relLBAStart*512 + byteCnt) <= byteLen);
	return disk->Read(lbaStart + relLBAStart, byteCnt, buf);
}

bool GptPartition::IsValid() const
{
	if (type[0]==0 && type[1]==0)
		return false;
	return true;
};

Gpt::Gpt():
	parts(nullptr)
{
}

bool Gpt::Init(Disk* disk)
{
	unique_array<uint64_t> buf(new uint64_t[512*33/8]);
	if (disk->GetCapacity() < 512*34)
		return false;
    if (disk->Read(1, 512*33, (BYTE*)buf.get())==false)
    	return false;
	if (buf[0] != 0x5452415020494645ULL) // GPT header signature
		return false;
	if (buf[9] != 2)
		return false;
	// TODO: Check CRC's
	diskGuid[0]=buf[7];
    diskGuid[1]=buf[8];
    numPartitions = (uint32_t)buf[10];
    if (numPartitions<0 || numPartitions > 128)
    	return false;
	uint32_t entrySize = ((uint32_t*)(buf.get()))[21];
	if (entrySize != 128)
		return false;
    parts.reset(new GptPartition[numPartitions]);
    if (parts.get() == nullptr)
    	return false;
    for (int i=0;i<numPartitions;i++)
	{
		GptPartition& part = parts[i];
		int base = 512/8+128/8*i;
		new (&part) GptPartition(disk, &buf[base], &buf[base+2], buf[base+4], buf[base+5], buf[base+6], (uint16_t*)&buf[base+7]);
	}
	return true;
}
