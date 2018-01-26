#include "mbr.h"

#include "kassert.h"

bool Mbr::Init(Disk* disk)
{
	BYTE mbrBuf[512];
	if (disk->Read(0, 512, mbrBuf)==false)
		return false;
	if (mbrBuf[510] != 0x55)
		return false;
	if (mbrBuf[511] != 0xAA)
		return false;
	for (int currentPartition = 0; currentPartition < 4; currentPartition++)
	{
		const BYTE* mbrBase = mbrBuf + 446 + currentPartition * 16;
		MbrPartition& partition = partitions[currentPartition];
		partition.disk = disk;
		partition.status = *(mbrBase+0);
		partition.partition_type = *(mbrBase+4);
		partition.lbaStart = *(uint32_t *)(mbrBase+8);
		partition.byteLen = 512ULL*(*(uint32_t *)(mbrBase+12));
	}
	return true;
}

bool MbrPartition::Read(uint64_t relLBAStart, uint64_t byteCnt, BYTE* buf)
{
	if (IsValid()==false)
		return false;
	kassert((relLBAStart*512 + byteCnt) <= byteLen);
	return disk->Read(lbaStart + relLBAStart, byteCnt, buf);
}

bool MbrPartition::IsValid() const
{
	return status==0x80 || status==0x00;
}
