/**
 * Task Loader
 */

#include <stddef.h>
#include <stdint.h>
#include "../device/ata.h"
#include "kvector.h"
#include "loader.h"
#include "util.h"

#define TASK_LOAD_ADDR 0xf000

extern kernel_vector_t kernel_vectors[];

typedef void (*task_t)(kernel_vector_t[]);

typedef struct {
    char* name;
    uint32_t sector;
} task_map_t;

task_map_t tasks[] = {
    { "taska", 13 },
    { "taskb", 14 },
    { NULL,     0 }
};

int lookup_task_sector(const char* name) {
    for (task_map_t *t = tasks; t->name != NULL; t++) {
        if (strcmp(t->name, name) == 0) {
            return t->sector;
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
        task_t task = (task_t)TASK_LOAD_ADDR;
        task(kernel_vectors);
    }

    return load_result;
}
