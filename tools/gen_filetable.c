/**
 * Generate a file table for the OS image
 * 
 * Usage: gen_filetable <output_file> <name1:sector1> <name2:sector2> ...
 * 
 * File table format (512 bytes):
 *   - 4 bytes: number of entries (little-endian)
 *   - For each entry (24 bytes):
 *     - 16 bytes: null-terminated name
 *     - 4 bytes: sector number (little-endian)
 *     - 4 bytes: size in sectors (little-endian, currently always 1)
 *   - Remaining bytes: zero-padded
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SECTOR_SIZE 512
#define NAME_SIZE 16
#define ENTRY_SIZE 24
#define MAX_ENTRIES ((SECTOR_SIZE - 4) / ENTRY_SIZE)

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <output_file> [name:sector ...]\n", argv[0]);
        return 1;
    }

    const char* output_file = argv[1];
    int num_entries = argc - 2;

    if (num_entries > MAX_ENTRIES) {
        fprintf(stderr, "Too many entries (max %d)\n", MAX_ENTRIES);
        return 1;
    }

    // Create a 512-byte buffer, zero-initialized
    uint8_t buffer[SECTOR_SIZE] = {0};

    // Write number of entries (4 bytes, little-endian)
    buffer[0] = num_entries & 0xFF;
    buffer[1] = (num_entries >> 8) & 0xFF;
    buffer[2] = (num_entries >> 16) & 0xFF;
    buffer[3] = (num_entries >> 24) & 0xFF;

    // Write each entry
    for (int i = 0; i < num_entries; i++) {
        char* arg = argv[i + 2];
        char* colon = strchr(arg, ':');
        if (!colon) {
            fprintf(stderr, "Invalid entry format: %s (expected name:sector)\n", arg);
            return 1;
        }

        // Parse name and sector
        *colon = '\0';
        const char* name = arg;
        uint32_t sector = atoi(colon + 1);

        if (strlen(name) >= NAME_SIZE) {
            fprintf(stderr, "Name too long: %s (max %d chars)\n", name, NAME_SIZE - 1);
            return 1;
        }

        // Calculate offset for this entry
        int offset = 4 + (i * ENTRY_SIZE);

        // Write name (16 bytes, null-padded)
        strncpy((char*)&buffer[offset], name, NAME_SIZE);

        // Write sector number (4 bytes, little-endian)
        buffer[offset + 16] = sector & 0xFF;
        buffer[offset + 17] = (sector >> 8) & 0xFF;
        buffer[offset + 18] = (sector >> 16) & 0xFF;
        buffer[offset + 19] = (sector >> 24) & 0xFF;

        // Write size (4 bytes, little-endian) - always 1 for now
        buffer[offset + 20] = 1;
        buffer[offset + 21] = 0;
        buffer[offset + 22] = 0;
        buffer[offset + 23] = 0;
    }

    // Write to file
    FILE* f = fopen(output_file, "wb");
    if (!f) {
        perror("Failed to open output file");
        return 1;
    }

    if (fwrite(buffer, 1, SECTOR_SIZE, f) != SECTOR_SIZE) {
        perror("Failed to write output file");
        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}

