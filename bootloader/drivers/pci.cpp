#include "pci.h"

DWORD PciDevice::ConfigReadDword(BYTE reg)
{
	if (reg > 15)
	{
		pstring("Invalid register given to PciDevice::ConfigReadDword!");
		return 0;
	}
	DWORD lbus  = (DWORD)bus;
	DWORD lslot = (DWORD)slot;
	DWORD lfunc = (DWORD)function;

	DWORD address = (DWORD)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (reg << 2) | ((DWORD)0x80000000));

	/* write out the address */
	PortIO::OutD(0xCF8, address);
	/* read in the data */
	return PortIO::InD(0xCFC);
}

WORD PciDevice::GetVendorID()
{
	return ConfigReadDword(0) & 0xFFFF;
}

BYTE PciDevice::GetBaseClass()
{
	return (ConfigReadDword(2) >> 24) & 0xFF;
}

BYTE PciDevice::GetHeaderType()
{
	return (ConfigReadDword(3) >> 16) & 0xFF;
}

BYTE PciDevice::GetSubClass()
{
	return (ConfigReadDword(2) >> 16) & 0xFF;
}

BYTE PciDevice::GetSecondaryBus()
{
	return (ConfigReadDword(6) >> 8) & 0xFF;
}

DWORD PciDevice::GetBar(int bar)
{
	if (bar<0 || bar >5)
	{
		pstring("Invalid bar number given to pciGetBar.");
		return 0;
	}
	return ConfigReadDword(bar+4);
}

int PciDevice::CheckFunction(PciDevice* array, int max_length)
{
	BYTE baseClass = GetBaseClass();
	BYTE subClass = GetSubClass();
	if( (baseClass == 0x06) && (subClass == 0x04) )
		return CheckBus(GetSecondaryBus(), array, max_length);
	else
	{
		array[0]=(*this);
		return 1;
	}
}

int PciDevice::CheckDevice(BYTE bus, BYTE device, PciDevice* array, int max_length)
{
	PciDevice tmpDev;
	tmpDev.bus = bus;
	tmpDev.slot = device;
	tmpDev.function = 0;
	if(tmpDev.GetVendorID() == 0xFFFF)
		return 0;        // Device doesn't exist
	int res, added=0;
	res = tmpDev.CheckFunction(array, max_length);
	array += res;
	max_length -= res;
	added += res;
	BYTE headerType = tmpDev.GetHeaderType();
	if( (headerType & 0x80) != 0)
	{
		/* It is a multi-function device, so check remaining functions */
		for(BYTE function = 1; (function < 8) && (max_length > 0); function++)
		{
			tmpDev.function = function;
			if(tmpDev.GetVendorID() != 0xFFFF)
			{
				res = tmpDev.CheckFunction(array, max_length);
				array += res;
				max_length -= res;
				added += res;
			}
		}
	}
	return added;
}

int PciDevice::CheckBus(BYTE bus, PciDevice* array, int max_length)
{
	int added = 0;
	for(BYTE device = 0; (device < 32) && (max_length > 0) ; device++)
	{
		const int res = CheckDevice(bus, device, array, max_length);
		array += res;
		max_length -= res;
		added += res;
	}
	return added;
}

int PciDevice::EnumerateDevices(PciDevice* array, int max_length)
{
	PciDevice tmpDev;
	tmpDev.bus=0;
	tmpDev.slot=0;
	tmpDev.function=0;
	BYTE headerType = tmpDev.GetHeaderType();
	if( (headerType & 0x80) == 0)
	{
		/* Single PCI host controller */
		return CheckBus(0, array, max_length);
	}
	else
	{
		int added = 0;
		/* Multiple PCI host controllers */
		for(BYTE function = 0; (function < 8) && (max_length > 0) ; function++)
		{
			tmpDev.function=function;
			if(tmpDev.GetVendorID() == 0xFFFF)
				break;
			const int res = CheckBus(function, array, max_length);
			array += res;
			max_length -= res;
			added += res;
		}
		return added;
	}
}
