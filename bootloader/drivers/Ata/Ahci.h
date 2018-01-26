#ifndef AHCI_H
#define AHCI_H

#include "drivers/pci.h"
#include "AhciDefs.h"
#include "drivers/Disk.h"
#include "memory/MemoryManager/MemoryManager.h"
#include "shared_ptr/shared_ptr.h"

class AhciController
{
public:
	PciDevice pciDev;
	int numPI; // How many ports are implemented?
	int whichst[32]; // whichst[i] holds which (like first, second, ...) implemented port *i* is, or -1, if it is not implemented.
	volatile AhciHbaMem* hbaMem;
	uint64_t ahciBaseLin;
	uint64_t ahciMemLength;
private:
	void MemoryInit();
public:
	AhciController();
	bool Init(PciDevice _pciDev, uint64_t hbaMemPhy);
	void Dest();
	AhciController(const AhciController&) = delete;
	AhciController(AhciController&&) = default;
	~AhciController();
	void StartEngines(int port);
	void StopEngines(int port);
};

class AhciDisk : public Disk
{
public:
	shared_ptr<AhciController> controller;
	volatile AhciHbaPort* portMem; // Belongs to *controller*
	AhciCmdHeader* cmdHeader;
	AhciCmdTable* cmdTable;
	AhciReceivedFIS* receivedFIS;
	int port;
	bool lba48;
	uint64_t capacity; // In bytes
private:
	static int EnumerateDisksInController(const shared_ptr<AhciController> controller, AhciDisk* disks, int max_length);
	void PrintErrorInfo(const char* op);
public:
	AhciDisk();
	bool Init(shared_ptr<AhciController> _controller, int _port);
	AhciDisk(const AhciDisk&) = delete;
	AhciDisk(AhciDisk&&) = default;
	AhciDisk& operator=(AhciDisk&&) = default;
	~AhciDisk() = default; // AhciDisk does not own any resource.
	// Memory manager and the heap must be initialized before Ahci disks can be enumerated! Ahci uses memory mapped i/o!
	static int EnumerateDisks(AhciDisk* disks, int max_length);
	bool Identify(uint16_t buf[256]);
	virtual bool Read(uint64_t lbaStart, uint64_t sectorCnt, uint8_t* buf);
	virtual uint64_t GetCapacity(); // In bytes
};

#endif
