#include <stddef.h>
#include <stdint.h>
#include "console.h"

#define ATA0_DATA         0x1F0
#define ATA0_RD_ERROR     0x1F1
#define ATA0_WR_FEATURES  0x1F1
#define ATA0_SECTOR_COUNT 0x1F2
#define ATA0_LBA_00       0x1F3
#define ATA0_LBA_08       0x1F4
#define ATA0_LBA_16       0x1F5
#define ATA0_LBA_24_HEAD  0x1F6
#define ATA0_RD_STATUS    0x1F7
#define ATA0_WR_COMMAND   0x1F7

#define DRV_HEAD_REG_MASK 0b10100000
#define DRV_HEAD_REG_LBA  0b01000000
#define DRV_HEAD_REG_DRV1 0b00010000

#define CMD_RD_SECTORS    0x20
#define STATUS_BUSY       0x80
#define STATUS_DRQ        0x08
#define STATUS_ERR        0x01

#define ERR_NOTHING       0x01
#define ERR_FORMATTER_DEV 0x02
#define ERR_SECTOR_BUF    0x03
#define ERR_ECC_CIRCUITRY 0x04
#define ERR_CONTROL       0x05
#define ERR_DRV1_FAILED   0x80


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

void port_outb(uint16_t port, uint8_t byte) {
    asm("out dx, al"
        : // no output data
        : "Nd"(port), "a"(byte));
}

uint8_t port_inb(uint16_t port) {
    uint8_t byte;
    asm("in ax, dx"
        : "=a"(byte)
        : "Nd"(port));
    return byte;
}

uint16_t port_inw(uint16_t port) {
    uint16_t word;
    asm("inw ax, dx"
        : "=a"(word)
        : "Nd"(port));
    return word;
}

void wait_on_busy() {
    uint8_t busy = port_inb(ATA0_RD_STATUS) & STATUS_BUSY;
    while (busy) {
        asm("pause"::);
        busy = port_inb(ATA0_RD_STATUS) & STATUS_BUSY;
    };
}

void handle_error() {
    print("ATA Error: ");

    uint8_t error = port_inb(ATA0_RD_ERROR);
    char hex[3] = "__";
    to_hexb(error, hex);
    print(hex);

    switch (error) {
        case 0:                 print("0"); break;      
        case ERR_NOTHING:       print("ERR_NOTHING"); break;      
        case ERR_FORMATTER_DEV: print("ERR_FORMATTER_DEV"); break;
        case ERR_SECTOR_BUF:    print("ERR_SECTOR_BUF"); break;   
        case ERR_ECC_CIRCUITRY: print("ERR_ECC_CIRCUITRY"); break;    
        case ERR_CONTROL:       print("ERR_CONTROL"); break;      
        case ERR_DRV1_FAILED:   print("ERR_DRV1_FAILED"); break;  
    }
}

void read_sectors(uint32_t start, size_t count, uint16_t* dest) {
    wait_on_busy();

    port_outb(ATA0_LBA_00, (uint8_t)start);
    port_outb(ATA0_LBA_08, (uint8_t)(start >> 8));
    port_outb(ATA0_LBA_16, (uint8_t)(start >> 16));
    port_outb(ATA0_LBA_24_HEAD, (uint8_t)(start >> 24 | DRV_HEAD_REG_MASK | DRV_HEAD_REG_LBA));
    port_outb(ATA0_SECTOR_COUNT, count);
    port_outb(ATA0_WR_COMMAND, CMD_RD_SECTORS);

    uint8_t status;
    for (int c = 0; c < count; c++) {
        wait_on_busy();
        status = port_inb(ATA0_RD_STATUS);

        if (status & STATUS_ERR) {
            handle_error();
        }

        if (status & STATUS_DRQ) {
            for (int i = 0; i < 256; i++, dest++) {
                *dest = port_inw(ATA0_DATA);
            }
        }
    }
}
