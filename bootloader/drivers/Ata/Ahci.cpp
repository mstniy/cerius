#include "Ahci.h"

#include "shared_ptr/unique_ptr.h"
#include "kout.h"

AhciController::AhciController() :
	numPI(0),
	hbaMem(nullptr),
	ahciBaseLin(0),
	ahciMemLength(0)
{
}


bool AhciController::Init(PciDevice _pciDev, uint64_t hbaMemPhy)
{
	Dest();

	pciDev = _pciDev;
	// sizeof(AhciHbaMem) is smaller than 2*4096, with proper alignment, <3*4096.
	// TODO: Properly calculate how much linear space is required.
	uint64_t _hbaMemLin = LinearSpaceManager::allocAslr(3*4096); // We're wasting linear space here, but it should be fine.
	kassert(_hbaMemLin != 0);
	const uint64_t padding = hbaMemPhy % 4096;
	const uint64_t phyAligned = hbaMemPhy - padding;
	Pager::pagein(phyAligned, _hbaMemLin, 3*4096, PagePrefs::pcd); // Disable cpu cache, since this is memory mapped i/o.
	hbaMem = (AhciHbaMem*)(_hbaMemLin+padding);
	if (hbaMem->cap & (1<<18) == 0) // Supports AHCI only
		hbaMem->ghc |= 1U<<31; // AHCI Enable
	if (hbaMem->cap & (1<<31) == 0) // Supports 64-bit addressing
		return false;
	hbaMem->ghc &= ~2; // Disable interrupts, because the bootloader doesn't use them

	const uint32_t pi = hbaMem->pi; // Avoid reading from uncached memory every time
    numPI = 0;
    for (int i=0;i<32;i++)
    	if (pi & (1U<<i))
			whichst[i]=numPI++;
		else
			whichst[i]=-1;
	if (numPI == 0)
		return false;
	MemoryInit();
	return true;
}

void AhciController::MemoryInit()
{
	const uint32_t pi = hbaMem->pi;
    ahciMemLength = numPI*(1024+256+128+4*128); // Command list + received fis + command table descriptor + prdt entry
    if (ahciMemLength % 4096)
    	ahciMemLength += 4096 - ahciMemLength % 4096;
    const uint64_t ahciBasePhy = PhysicalSpaceManager::alloc(ahciMemLength); // AHCI memory must be physically contiguous
    ahciBaseLin = LinearSpaceManager::allocAslr(ahciMemLength);
    kassert(ahciBasePhy!=0 && ahciBaseLin != 0);
    Pager::pagein(ahciBasePhy, ahciBaseLin, ahciMemLength, PagePrefs::pcd);
    for (int i=0;i<32;i++)
    	if (pi & (1U<<i))
		{
			StopEngines(i);
			volatile AhciHbaPort& port = hbaMem->ports[i];
			const uint64_t cmdHeaderOffset = 1024*whichst[i];
			const uint64_t receivedFISOffset = numPI*1024 + whichst[i]*256;
			const uint64_t cmdTableOffset  = numPI*(1024+256) + whichst[i]*128;
			volatile AhciCmdHeader& cmdHeader = *(AhciCmdHeader*)(ahciBaseLin+cmdHeaderOffset);
            port.clb = ahciBasePhy + cmdHeaderOffset;
            port.fb = ahciBasePhy + receivedFISOffset;
            cmdHeader.ctba = ahciBasePhy + cmdTableOffset;
		}
}

void AhciController::StartEngines(int port)
{
	volatile AhciHbaPort& portMem = hbaMem->ports[port];
	kassert((portMem.cmd & 1)==0); // Start
	while (portMem.cmd & (1<<15)) // Command list running
		;

	portMem.cmd |= (1<<4); // FIS receive enabled
	portMem.cmd |= 1; // Start
}

void AhciController::StopEngines(int port)
{
	volatile AhciHbaPort& portMem = hbaMem->ports[port];
	portMem.cmd &= ~1; // Start

	while (portMem.cmd & (1<<15)) // Command list running
		;

	portMem.cmd &= ~(1<<4); // FIS receive enabled

	while(portMem.cmd & (1<<14)) // FIS receive running
		;
}

void AhciController::Dest()
{
	if (hbaMem != nullptr)
	{
		const uint64_t lin = (uint64_t) hbaMem;
		const uint64_t hbaMemAligned = lin - lin%4096;
        Pager::pageout((void*)hbaMemAligned, 3*4096);
        LinearSpaceManager::free(hbaMemAligned, 3*4096);
        hbaMem = nullptr;
	}
	if (ahciBaseLin != 0)
	{
		MemoryManager::free((void*)ahciBaseLin, ahciMemLength);
		ahciBaseLin = 0;
	}
}

AhciController::~AhciController()
{
	Dest();
}

AhciDisk::AhciDisk():
	portMem(nullptr),
	cmdHeader(nullptr),
	cmdTable(nullptr),
	receivedFIS(nullptr)
{
}

bool AhciDisk::Init(shared_ptr<AhciController> _controller, int _port)
{
	controller = _controller;
	port = _port;

	portMem = &controller->hbaMem->ports[port];
	const uint32_t ssts = portMem->ssts;
	const uint8_t ipm = (ssts >> 8) & 0x0F;
	const uint8_t det = ssts & 0x0F;

	if (det != 3) // Physical communication established
		return false;
	if (ipm != 1) // Active power state
		return false;
	if (portMem->sig != AHCI_ATA_SIG)
		return false;
	controller->StartEngines(port);

	const int whichst = controller->whichst[port];
	kassert(whichst != -1);
	cmdHeader = (AhciCmdHeader*)(controller->ahciBaseLin + whichst*1024);
	receivedFIS = (AhciReceivedFIS*)(controller->ahciBaseLin + controller->numPI*1024 + whichst*256);
	cmdTable = (AhciCmdTable*)(controller->ahciBaseLin + controller->numPI*(1024+256) + whichst*128);
	auto buf = unique_array<uint16_t>(new uint16_t[256]);
	if (Identify(buf.get()))
	{
		lba48 = buf[83] & (1<<10); // 48-bit Address features set supported
		if (lba48)
			capacity = 512*(*(uint64_t*)&buf[100]); // Maximum user LBA for 48-bit Address feature set
		else
			capacity = 512*buf[60]; // Total number of user addressable sectors
		return true;
	}
	return false;
}

int AhciDisk::EnumerateDisks(AhciDisk* disks, int max_length)
{
	int disks_length = 0;
    PciDevice pci_devices[100];
    const int pci_dev_cnt = PciDevice::EnumerateDevices(pci_devices, 100);

	for (int i=0;i<pci_dev_cnt;i++)
	{
		PciDevice& dev = pci_devices[i];
		if ((dev.GetBaseClass() != 1) || (dev.GetSubClass() != 6))
			continue;
		uint64_t hbaMemPhy = dev.GetBar(5);
		if (hbaMemPhy == 0)
			continue;
		shared_ptr<AhciController> controller(new AhciController);
		if (controller->Init(dev, hbaMemPhy) == false)
			continue;
		disks_length += AhciDisk::EnumerateDisksInController(controller, disks+disks_length, max_length-disks_length);
		if (disks_length >= max_length)
			break;
	}
	return disks_length;
}

int AhciDisk::EnumerateDisksInController(shared_ptr<AhciController> controller, AhciDisk* disks, int max_length)
{
	int curlength = 0;
	for (int i=0;i<32;i++)
	{
		if (controller->hbaMem->pi && (1U<<i))
		{
			if (disks[curlength].Init(controller, i))
				curlength++;
			if (curlength >= max_length)
				break;
		}
	}
	return curlength;
}

void AhciDisk::PrintErrorInfo(const char* op)
{
	kout << "Ahci error during operation " << op << "\n";
	kout << "PxIS: " << portMem->is;
	kout << ", PxSERR: "<< portMem->serr;
	kout << ", PxSSTS: " << portMem->ssts;
	kout << ", PxTFD: " << portMem->tfd << "\n";
}

bool AhciDisk::Read(uint64_t lbaStart, uint64_t byteCnt, uint8_t* buf)
{
	for (int i=0;i<byteCnt;i++)
		buf[i]=0;
	kassert(uint64_t(buf)%2==0);
	kassert(byteCnt%512 == 0);
	kassert(byteCnt <= 4*1024*1024);
	kassert(lbaStart*512+byteCnt <= capacity);

	const uint64_t sectorCnt = byteCnt / 512;

	portMem->is = 0;  // Clear pending interrupt bits

	cmdHeader->cfl = sizeof(SataH2DRegisterFIS)/sizeof(uint32_t);	// Command FIS size
	cmdHeader->a = 0;
	cmdHeader->w = 0;
	cmdHeader->p = 1;
	cmdHeader->r = 0;
	cmdHeader->b = 0;
	cmdHeader->c = 0;
	cmdHeader->rsv0 = 0;
	cmdHeader->pmp = 0;
	cmdHeader->prdtl = 1;	// PRDT entries count // TODO: We can use up to 4 prd's, we allocate memory for them in MemoryInit().
	cmdHeader->prdbc = 0;
    for (int i=0;i<4;i++)
    	cmdHeader->rsv1[i]=0;

	cmdTable->prdt_entry[0].dba = Pager::lintophy(buf); //TODO: The given linear range may not be continuous on physical memory. We need to factor that in.
	cmdTable->prdt_entry[0].dbc = byteCnt-1;
	cmdTable->prdt_entry[0].i = 0;

	// Setup command
	SataH2DRegisterFIS& cmdfis = *(SataH2DRegisterFIS*)(&cmdTable->cfis);

	cmdfis = {};
	cmdfis.fis_type = 0x27; // Host to device register fis
	cmdfis.c = 1;	// Command
	if (lba48)
		cmdfis.command = ATA_CMD_READ_DMA_EXT;
	else
		cmdfis.command = ATA_CMD_READ_DMA;
	cmdfis.lba0 = (uint8_t)lbaStart;
	cmdfis.lba1 = (uint8_t)(lbaStart>>8);
	cmdfis.lba2 = (uint8_t)(lbaStart>>16);
	cmdfis.device = 1<<6;	// LBA mode
	if (lba48 == false)
	{
        cmdfis.device |= (lbaStart>>24) & 0xF;
	}
	else
	{
		cmdfis.lba3 = (uint8_t)(lbaStart>>24);
		cmdfis.lba4 = (uint8_t)(lbaStart>>32);
		cmdfis.lba5 = (uint8_t)(lbaStart>>40);
	}
	cmdfis.count = sectorCnt;

	// The below loop waits until the port is no longer busy before issuing a new command
	int spin;
	for (spin=0;spin<1000000;spin++)
	{
		if (!(portMem->tfd & 0x88)) // Busy or DRQ
			break;
	}
	if (spin == 1000000)
	{
		kout << "Ahci port is hung\n";
		return false;
	}

	portMem->ci = 1;	// Issue command

	// Wait for completion
	while (1)
	{
		if (portMem->is & 0x79000000) // Various errors
		{
			PrintErrorInfo("Read");
			return false;
		}
		if ((portMem->ci & 1) == 0)
			break;
	}
	return cmdHeader->prdbc == byteCnt; // Check for possible underflow
}

bool AhciDisk::Identify(uint16_t buf[256])
{
	for (int i=0;i<256;i++)
		buf[i]=0;
	portMem->is = 0;  // Clear pending interrupt bits

	cmdHeader->cfl = sizeof(SataH2DRegisterFIS)/sizeof(uint32_t);	// Command FIS size
	cmdHeader->a = 0;
	cmdHeader->w = 0;
	cmdHeader->p = 1;
	cmdHeader->r = 0;
	cmdHeader->b = 0;
	cmdHeader->c = 0;
	cmdHeader->rsv0 = 0;
	cmdHeader->pmp = 0;
	cmdHeader->prdtl = 1;
	cmdHeader->prdbc = 0;
    for (int i=0;i<4;i++)
    	cmdHeader->rsv1[i]=0;

	cmdTable->prdt_entry[0].dba = Pager::lintophy(buf); //TODO: The given linear range may not be continuous on physical memory. We need to factor that in.
	cmdTable->prdt_entry[0].dbc = 511;
	cmdTable->prdt_entry[0].i = 0;

	// Setup command
	SataH2DRegisterFIS& cmdfis = *(SataH2DRegisterFIS*)(&cmdTable->cfis);

	cmdfis = {};
	cmdfis.fis_type = 0x27; // Host to device register fis
	cmdfis.c = 1;	// Command
	cmdfis.command = 0xEC; // Identify device

	// The below loop waits until the port is no longer busy before issuing a new command
	int spin;
	for (spin=0;spin<1000000;spin++)
	{
		if (!(portMem->tfd & 0x88)) // Busy or DRQ
			break;
	}
	if (spin == 1000000)
	{
		kout << "Ahci port is hung\n";
		return false;
	}

	portMem->ci = 1;	// Issue command

	// Wait for completion
	while (1)
	{
		if (portMem->is & 0x79000000) // Various errors
		{
			PrintErrorInfo("Identify");
			return false;
		}
		if ((portMem->ci & 1) == 0)
			break;
	}
	return cmdHeader->prdbc == 512; // Check for possible underflow
}

uint64_t AhciDisk::GetCapacity()
{
    return capacity;
}
