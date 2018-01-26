#ifndef ATADEFS_H
#define ATADEFS_H

#define ATA_SR_BSY     0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Device fault
#define ATA_SR_DRQ     0x08    // Data request ready
#define ATA_SR_ERR     0x01    // Error

#define ATA_ER_ICRC     0x80    // Interface CRC error
#define ATA_ER_WP       0x40    // Write protect
#define ATA_ER_UNC      0x40    // Data uncorrectable
#define ATA_ER_MC       0x20    // Media change
#define ATA_ER_IDNF     0x10    // Address not user accessible
#define ATA_ER_MCR      0x08    // Media change request
#define ATA_ER_ABRT     0x04    // Abort
#define ATA_ER_NM       0x02    // No media

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_IDENTIFY          0xEC

#define ATAPI_CMD_READ       0xA8
#define ATAPI_CMD_EJECT      0x1B

#define ATA_IDENT_MAX_LBA      60
#define ATA_IDENT_MAX_LBA_EXT  100

#endif // ATADEFS_H
