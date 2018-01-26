#ifndef AHCIDEFS_H
#define AHCIDEFS_H

#include "SataDefs.h"

#define AHCI_ATA_SIG 0x00000101 // Defined jointly by ATA and AHCI specs
#define AHCI_ATAPI_SIG 0xEB140101

struct AhciReceivedFIS
{
	SataDMASetupFIS	dsfis;
	uint8_t         pad0[4];

	SataPIOSetupFIS psfis;
	uint8_t         pad1[12];

	SataD2HRegisterFIS rfis;
	uint8_t         pad2[4];

	uint16_t sdbfis; // Set Device Bit FIS

	uint8_t         ufis[64];

	uint8_t   	rsv[0x100-0xA0];
};

struct AhciCmdHeader
{
	uint8_t  cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
	uint8_t  a:1;		// ATAPI
	uint8_t  w:1;		// Write, 1: H2D, 0: D2H
	uint8_t  p:1;		// Prefetchable

	uint8_t  r:1;		// Reset
	uint8_t  b:1;		// BIST
	uint8_t  c:1;		// Clear busy upon R_OK
	uint8_t  rsv0:1;		// Reserved
	uint8_t  pmp:4;		// Port multiplier port

	uint16_t prdtl;		// Physical region descriptor table length in entries

	uint32_t prdbc;		// Physical region descriptor byte count transferred

	uint64_t ctba;		// Command table descriptor base address

	uint32_t rsv1[4];	// Reserved
};

struct AhciPRDTEntry
{
	uint64_t dba;		// Data base address
	uint32_t rsv0;		// Reserved
	uint32_t dbc:22;		// Byte count, 4M max
	uint32_t rsv1:9;		// Reserved
	uint32_t i:1;		// Interrupt on completion
};

struct AhciCmdTable
{
	uint8_t  cfis[64];	// Command FIS
	uint8_t  acmd[16];	// ATAPI command, 12 or 16 bytes
	uint8_t  rsv[48];	// Reserved
	AhciPRDTEntry prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
};

struct AhciHbaPort
{
	uint64_t clb;		// 0x00, command list base address, 1K-byte aligned
	uint64_t fb;		// 0x08, FIS base address, 256-byte aligned
	uint32_t is;		// 0x10, interrupt status
	uint32_t ie;		// 0x14, interrupt enable
	uint32_t cmd;		// 0x18, command and status
	uint32_t rsv0;		// 0x1C, Reserved
	uint32_t tfd;		// 0x20, task file data
	uint32_t sig;		// 0x24, signature
	uint32_t ssts;		// 0x28, SATA status (SCR0:SStatus)
	uint32_t sctl;		// 0x2C, SATA control (SCR2:SControl)
	uint32_t serr;		// 0x30, SATA error (SCR1:SError)
	uint32_t sact;		// 0x34, SATA active (SCR3:SActive)
	uint32_t ci;		// 0x38, command issue
	uint32_t sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	uint32_t fbs;		// 0x40, FIS-based switch control
	uint32_t rsv1[11];	// 0x44 ~ 0x6F, Reserved
	uint32_t vendor[4];	// 0x70 ~ 0x7F, vendor specific
};

struct AhciHbaMem
{
	// 0x00 - 0x2B, Generic Host Control
	uint32_t cap;		// 0x00, Host capability
	uint32_t ghc;		// 0x04, Global host control
	uint32_t is;		// 0x08, Interrupt status
	uint32_t pi;		// 0x0C, Port implemented
	uint32_t vs;		// 0x10, Version
	uint32_t ccc_ctl;	// 0x14, Command completion coalescing control
	uint32_t ccc_pts;	// 0x18, Command completion coalescing ports
	uint32_t em_loc;		// 0x1C, Enclosure management location
	uint32_t em_ctl;		// 0x20, Enclosure management control
	uint32_t cap2;		// 0x24, Host capabilities extended
	uint32_t bohc;		// 0x28, BIOS/OS handoff control and status

	// 0x2C - 0x9F, Reserved
	uint8_t  rsv[0xA0-0x2C];

	// 0xA0 - 0xFF, Vendor specific registers
	uint8_t  vendor[0x100-0xA0];

	// 0x100 - 0x10FF, Port control registers
	AhciHbaPort ports[1];	// 1 ~ 32
};

#endif
