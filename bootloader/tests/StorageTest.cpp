#include "drivers/Ata/Ahci.h"
#include "drivers/Ata/Ide.h"
#include "drivers/mbr.h"
#include "drivers/Gpt.h"
#include "common/kout.h"

void StorageTest()
{
	auto buf = unique_array<BYTE>(new BYTE[512]);
    AhciDisk adisks[10];
    IdeDisk idisks[10];
    kout << "Enumerating Ahci disks...\n";
    int alen = AhciDisk::EnumerateDisks(adisks, 10);
    kout << "There are " << alen << " AHCI disks.\n";
    kout << "Enumerating Ide disks...\n";
    int ilen = IdeDisk::EnumerateDisks(idisks, 10);
    kout << "There are " << ilen << " IDE disks.\n";
	Disk* disks[20];
	for (int i=0;i<alen;i++)
		disks[i]=&adisks[i];
	for (int i=0;i<ilen;i++)
		disks[i]=&idisks[i];
	for (int i=0;i<alen+ilen;i++)
	{
		kout << "Disk "	<< i << ":\n";
        kout << "    Capacity: " << disks[i]->GetCapacity() << "\n";
        if (disks[i]->GetCapacity() < 512)
		{
			kout << "    Skipping.\n";
			continue;
		}
		for (int i=0;i<512;i++) buf[i]=0;
        if (disks[i]->Read(0, 512, buf.get()) == false)
        {
        	kout << "    Read failed.\n";
        	continue;
        }
		kout << "    Begins with ";
        for (int i=0;i<5;i++)
		{
			pbyte(buf[i]);
			kout << ' ';
		}
		kout << "\n";
		Gpt gpt;
		Mbr mbr;
        if (gpt.Init(disks[i]))
		{
        	kout << "    GPT partitioning scheme\n";
        	for (int i=0;i<gpt.numPartitions;i++)
			{
				GptPartition& partition = gpt.parts[i];
				if (partition.IsValid() == false)
					continue;
				kout << "      Partition " << i << ": ";
				kout << (partition.lbaStart*512) << " - " << partition.byteLen << ": ";
				for (int i=0;i<36;i++)
				{
					if (partition.name[i]==0)
						break;
					kout << (char)partition.name[i]; // Quick hack to display UTF16
				}
				kout << "\n";
			}
		}
		else if (mbr.Init(disks[i]))
		{
			kout << "    MBR partitioning scheme\n";
			for (int i=0; i<4; i++)
			{
				MbrPartition& partition = mbr.partitions[i];
				if (partition.IsValid() == false)
				{
					kout << "      Invalid partition.\n";
					continue;
				}
				kout << "      Partition " << i << ": ";
				kout << (partition.lbaStart*512) << " - " << partition.byteLen << "\n";
			}
		}
		else
			kout << "    Invalid GPT, MBR\n";
	}
}
