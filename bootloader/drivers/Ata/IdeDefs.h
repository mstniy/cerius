#ifndef IDEDEFS_H
#define IDEDEFS_H

#define IDE_ATA        0x00
#define IDE_ATAPI      0x01

#define ATA_MASTER     0xA0
#define ATA_SLAVE      0xB0

#define ATA_PRIMARY      0x00
#define ATA_SECONDARY    0x01

#define ATA_READ      0x00
#define ATA_WRITE     0x01

#define PATA_REG_DATA       0x00
#define PATA_REG_ERROR      0x01
#define PATA_REG_FEATURES   0x01
#define PATA_REG_SECCOUNT0  0x02
#define PATA_REG_LBA0       0x03
#define PATA_REG_LBA1       0x04
#define PATA_REG_LBA2       0x05
#define PATA_REG_HDDEVSEL   0x06
#define PATA_REG_COMMAND    0x07
#define PATA_REG_STATUS     0x07
#define PATA_REG_SECCOUNT1  0x08
#define PATA_REG_LBA3       0x09
#define PATA_REG_LBA4       0x0A
#define PATA_REG_LBA5       0x0B
#define PATA_REG_CONTROL    0x0C
#define PATA_REG_ALTSTATUS  0x0C
#define PATA_REG_DEVADDRESS 0x0D

#endif
