/**
 * Task Loader
 *
 * Reads a file table from sector 1 of the disk to find task locations.
 * File table format (512 bytes):
 *   - 4 bytes: number of entries
 *   - For each entry (24 bytes):
 *     - 16 bytes: null-terminated name
 *     - 4 bytes: sector number
 *     - 4 bytes: size in sectors
 */

#include <stddef.h>
#include <stdint.h>
#include "device/ata.h"
#include "kernel/vector.h"
#include "kernel/loader.h"
#include "lib/util.h"

#define TASK_LOAD_ADDR 0xf000
#define FILETABLE_SECTOR 1
#define FILETABLE_NAME_SIZE 16
#define FILETABLE_ENTRY_SIZE 24
#define FILETABLE_MAX_ENTRIES 21

extern kernel_vector_t kernel_vectors[];

typedef void (*task_fn_t)(kernel_vector_t[]);

// File table entry (matches the on-disk format)
typedef struct __attribute__((packed)) {
    char name[FILETABLE_NAME_SIZE];
    uint32_t sector;
    uint32_t size;
} filetable_entry_t;

// File table header
typedef struct __attribute__((packed)) {
    uint32_t num_entries;
    filetable_entry_t entries[FILETABLE_MAX_ENTRIES];
} filetable_t;

static filetable_t filetable;
static int filetable_loaded = 0;

static void load_filetable() {
    if (filetable_loaded) return;
    
    read_sectors(FILETABLE_SECTOR, 1, (uint32_t*)&filetable);
    filetable_loaded = 1;
}

int lookup_task_sector(const char* name) {
    load_filetable();
    
    for (uint32_t i = 0; i < filetable.num_entries; i++) {
        if (strcmp(filetable.entries[i].name, name) == 0) {
            return filetable.entries[i].sector;
        }
    }
    return -1;
}

int load_task(const char* name, uint32_t* dest) {
    int read_count = 0;

    uint32_t sector = lookup_task_sector(name);
    if (sector >= 0) {
        read_count = read_sectors(sector, 1, dest);
    }

    return (read_count == 1) ? 0 : -1;
}

int exec(const char* name) {
    // load
    int load_result = load_task(name, (uint32_t*)TASK_LOAD_ADDR);
    if (load_result == 0) {
        // execute
        task_fn_t task = (task_fn_t)TASK_LOAD_ADDR;
        task(kernel_vectors);
    }

    return load_result;
}
