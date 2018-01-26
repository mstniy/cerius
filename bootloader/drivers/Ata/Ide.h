#ifndef IDE_H
#define IDE_H

#include "drivers/pci.h"
#include "common/type.h"
#include "common/utils.h"
#include "common/kassert.h"
#include "AtaDefs.h"
#include "drivers/Disk.h"

class IdeDisk : public Disk
{
public:
	PciDevice con;
	bool primary;
	bool master;
	WORD port, conPort;
	bool lba48;
	uint64_t capacity;
public:
	IdeDisk() = default;
	bool Init(PciDevice _con, bool _primary, bool _master, WORD _port, WORD _conPort);
	static int EnumerateDisks(IdeDisk* array, int max_length);
	bool Identify(WORD buf[256]);
	virtual bool Read(uint64_t lbaStart, uint64_t byteCnt, uint8_t* buf);
	virtual uint64_t GetCapacity();
};

#endif
