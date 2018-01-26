#include "Ide.h"
#include "IdeDefs.h"

bool IdeDisk::Init(PciDevice _con, bool _primary, bool _master, WORD _port, WORD _conPort)
{
	con = _con;
	primary = _primary;
	master = _master;
	port = _port;
	conPort = _conPort;
	WORD identifyBuf[256];
	if (Identify(identifyBuf) && (identifyBuf[49] & (1<<9))) // LBA supported
	{
		lba48 = identifyBuf[83] & (1<<10); // 48-bit Address feature set supported
		if (lba48)
			capacity = 512*(*(uint64_t*)(identifyBuf+ATA_IDENT_MAX_LBA_EXT));
		else
			capacity = 512*(*(uint32_t*)(identifyBuf+ATA_IDENT_MAX_LBA));
		return true;
	}
	else
		return false;
}

int IdeDisk::EnumerateDisks(IdeDisk* ide_disks, int max_length)
{
	int ide_disks_length = 0;
	PciDevice pci_devices[100];
	const int pci_dev_cnt = PciDevice::EnumerateDevices(pci_devices, 100);

	for (int pci_dev_index=0; (pci_dev_index< pci_dev_cnt) && (ide_disks_length < max_length); pci_dev_index++)
	{
		PciDevice& pci_device = pci_devices[pci_dev_index];
		BYTE baseClass = pci_device.GetBaseClass();
		BYTE subClass  = pci_device.GetSubClass();

		if (baseClass != 0x01 || subClass != 0x01)
			continue;

		WORD pcPort, pcConPort, scPort, scConPort;
		pcPort=pci_device.GetBar(0);
		pcConPort=pci_device.GetBar(1);
		scPort=pci_device.GetBar(2);
		scConPort=pci_device.GetBar(3);
		if (pcPort < 2) pcPort = 0x1F0;
		if (pcConPort < 2) pcConPort = 0x3F6;
		if (scPort < 2) scPort = 0x170;
		if (scConPort < 2) scConPort = 0x376;
		for (int primary=0; primary<2; primary++)
		{
			for (int master=0; master<2; master++)
			{
				if (ide_disks_length < max_length)
				{
					const WORD port = primary ? pcPort : scPort;
					const WORD conPort = primary ? pcConPort : scConPort;
					IdeDisk& newDisk = ide_disks[ide_disks_length];
					if (newDisk.Init(pci_device, primary, master, port, conPort))
						ide_disks_length++;
				}
			}
		}
	}
	return ide_disks_length;
}

bool IdeDisk::Read(uint64_t lbaStart, uint64_t byteCnt, uint8_t* buf)
{
	kassert(uint64_t(buf)%2==0);
	kassert(byteCnt%512 == 0);
	kassert(byteCnt <= 4*1024*1024);
	kassert(lbaStart*512+byteCnt <= capacity);

	const uint64_t sectorCnt = byteCnt / 512;

	uint64_t curLBA = lbaStart;
	const uint64_t endLBA = lbaStart + sectorCnt; // Exclusive
	while (curLBA < endLBA)
	{
		int sectors_to_read = endLBA-curLBA;
		if (lba48 == false)
		{
			if (sectors_to_read > 255)
				sectors_to_read = 255;
			PortIO::OutB(port + PATA_REG_HDDEVSEL, 0xE0 | ((1-master) << 4) | ((curLBA >> 24) & 0x0F));
			PortIO::OutB(port + PATA_REG_SECCOUNT0, sectors_to_read);
			PortIO::OutB(port + PATA_REG_LBA0, curLBA);
			PortIO::OutB(port + PATA_REG_LBA1, curLBA >> 8);
			PortIO::OutB(port + PATA_REG_LBA2, curLBA >> 16);
			PortIO::OutB(port + PATA_REG_COMMAND, ATA_CMD_READ_PIO);
		}
		else
		{
			if (sectors_to_read > 65535)
				sectors_to_read = 65535;
			PortIO::OutB(port + PATA_REG_HDDEVSEL, 0xE0 | ((1-master) << 4));
			PortIO::OutB(port + PATA_REG_SECCOUNT0, (sectors_to_read>>8) & 0xFF);
			PortIO::OutB(port + PATA_REG_SECCOUNT0, sectors_to_read & 0xFF);
			PortIO::OutB(port + PATA_REG_LBA0, ((curLBA>>24) & 0xF));
			PortIO::OutB(port + PATA_REG_LBA0, curLBA & 0xF);
			PortIO::OutB(port + PATA_REG_LBA1, (curLBA >> 32) & 0xFF);
			PortIO::OutB(port + PATA_REG_LBA1, (curLBA >> 8) & 0xFF);
			PortIO::OutB(port + PATA_REG_LBA2, (curLBA >> 40) & 0xFF);
			PortIO::OutB(port + PATA_REG_LBA2, (curLBA >> 16) & 0xFF);
			PortIO::OutB(port + PATA_REG_COMMAND, ATA_CMD_READ_PIO_EXT);
		}
		for (int i=0; i<sectors_to_read; i++)
		{
			BYTE status;
			for (int i=0; i<3; i++)
				status = PortIO::InB(port + PATA_REG_STATUS);
			bool poll_success = false;
			while (1)
			{
				status = PortIO::InB(port + PATA_REG_STATUS);
				if ((status & ATA_SR_ERR) || (status & ATA_SR_DF))
					break;
				if (((status & ATA_SR_BSY) == 0) && (status & ATA_SR_DRQ))
				{
					poll_success = true;
					break;
				}
			}
			if (poll_success == false)
				return false;
			for (int i=0; i<256; i++) // A sector is 512 bytes.
				*(uint16_t*)(buf+2*i)=PortIO::InW(port + PATA_REG_DATA);
		}
		curLBA += sectors_to_read;
	}
	return true;
}

uint64_t IdeDisk::GetCapacity()
{
	return capacity;
}

bool IdeDisk::Identify(WORD buf[256])
{
	PortIO::OutB(port + PATA_REG_HDDEVSEL  , master ? ATA_MASTER : ATA_SLAVE);
	PortIO::OutB(port + PATA_REG_COMMAND   , ATA_CMD_IDENTIFY);
	for (int i=0; i<3; i++)
		PortIO::InB(port + PATA_REG_STATUS);
	BYTE status;
	bool poll_success = false;
	while (1)
	{
		status = PortIO::InB(port + PATA_REG_STATUS);
		if (status == 0) // Device does not exist.
			break;
		if (((status & ATA_SR_BSY) == 0) && (status & ATA_SR_DRQ))
		{
			poll_success = true;
			break;
		}
		if ((status & ATA_SR_ERR) || (status & ATA_SR_DF)) // Device reported error.
			break;
	}
	if (poll_success) // Device set DRQ
	{
		for (int i=0; i<256; i++)
			buf[i] = PortIO::InW(port + PATA_REG_DATA);
		return true;
	}
	else
		return false;
}
