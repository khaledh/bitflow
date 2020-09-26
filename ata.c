#include <stddef.h>
#include <stdint.h>
#include "console.h"
#include "port.h"

#define ATA0_BASE         0x1F0

#define REG_DATA          0
#define REG_ERROR         1
#define REG_FEATURES      1
#define REG_SECTOR_COUNT  2
#define REG_LBA_00        3
#define REG_LBA_08        4
#define REG_LBA_16        5
#define REG_LBA_24        6
#define REG_STATUS        7
#define REG_COMMAND       7

#define FLAG_LBA          0b01000000
#define FLAG_DRV1         0b00010000

#define CMD_READ_SECTORS  0x20
#define STATUS_BUSY       0x80
#define STATUS_DRQ        0x08
#define STATUS_ERR        0x01

// #define ERR_NOTHING       0x01
// #define ERR_FORMATTER_DEV 0x02
// #define ERR_SECTOR_BUF    0x03
// #define ERR_ECC_CIRCUITRY 0x04
// #define ERR_CONTROL       0x05
// #define ERR_DRV1_FAILED   0x80

void to_hexb(uint8_t value, char* buf) {
    char digits[17] = "01234567890abcdef";
    for (int i = 0; i < 2; i++) {
        buf[1 - i] = digits[(value >> (i * 4)) & 0x000F];
    }
}

void to_hexw(uint16_t value, char* buf) {
    char digits[17] = "01234567890abcdef";
    for (int i = 0; i < 4; i++) {
        buf[3 - i] = digits[(value >> (i * 4)) & 0x000F];
    }
}

// void handle_error() {
//     print("ATA Error: ");

//     uint8_t error = port_in8(ATA0_BASE + REG_ERROR);
//     char hex[3] = "__";
//     to_hexb(error, hex);
//     print(hex);

//     switch (error) {
//         case 0:                 print("0"); break;      
//         case ERR_NOTHING:       print("ERR_NOTHING"); break;      
//         case ERR_FORMATTER_DEV: print("ERR_FORMATTER_DEV"); break;
//         case ERR_SECTOR_BUF:    print("ERR_SECTOR_BUF"); break;   
//         case ERR_ECC_CIRCUITRY: print("ERR_ECC_CIRCUITRY"); break;    
//         case ERR_CONTROL:       print("ERR_CONTROL"); break;      
//         case ERR_DRV1_FAILED:   print("ERR_DRV1_FAILED"); break;  
//     }
// }

void wait_on_busy() {
    uint8_t busy = port_in8(ATA0_BASE + REG_STATUS) & STATUS_BUSY;
    while (busy) {
        asm("pause"::);
        busy = port_in8(ATA0_BASE + REG_STATUS) & STATUS_BUSY;
    };
}

int read_sectors(uint32_t start, size_t count, uint32_t* dest) {
    wait_on_busy();

    // send LBA block address
    port_out8(ATA0_BASE + REG_LBA_00, (uint8_t)start);
    port_out8(ATA0_BASE + REG_LBA_08, (uint8_t)(start >> 8));
    port_out8(ATA0_BASE + REG_LBA_16, (uint8_t)(start >> 16));
    port_out8(ATA0_BASE + REG_LBA_24, (uint8_t)(start >> 24 | FLAG_LBA));
    // send sector count
    port_out8(ATA0_BASE + REG_SECTOR_COUNT, count);
    // send command
    port_out8(ATA0_BASE + REG_COMMAND, CMD_READ_SECTORS);

    // read sectors
    size_t read_count = 0;
    uint16_t* ptr = (uint16_t*)dest;
    for (int c = 0; c < count; c++) {
        wait_on_busy();

        uint8_t status = port_in8(ATA0_BASE + REG_STATUS);
        if (status & STATUS_ERR) {
            // ideally we should read the Error register for more details
            break;
        }
        if ((status & STATUS_DRQ) == 0) {
            // this shouldn't happen; we may handle this differently later
            break;
        }
        // read sector's data in 16-bit words
        for (int i = 0; i < 256; i++, ptr++) {
            *ptr = port_in16(ATA0_BASE + REG_DATA);
        }
        read_count++;
    }

    return read_count;
}
