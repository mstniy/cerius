#ifndef PCI_H
#define PCI_H

#include "../common/type.h"
#include "display8025_col.h"
#include "../common/port_io.h"

class PciDevice
{
public:
	BYTE bus, slot, function;
private:
	int CheckFunction(PciDevice* array, int max_length);
	static int CheckDevice(BYTE bus, BYTE device, PciDevice* array, int max_length);
	static int CheckBus(BYTE bus, PciDevice* array, int max_length);
public:
	DWORD ConfigReadDword(BYTE reg);
	WORD  GetVendorID();
	BYTE  GetBaseClass();
	BYTE  GetHeaderType();
	BYTE  GetSubClass();
	BYTE  GetSecondaryBus();
	DWORD GetBar(int bar);

	static int EnumerateDevices(PciDevice* array, int max_length);
};


#endif
