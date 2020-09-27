#include <stddef.h>
#include <stdint.h>
#include "task.h"
#include "ata.h"

#define TASK_LOAD_ADDR 0xf000

typedef void (*task_t)(void);

typedef struct {
    char* name;
    uint32_t sector;
} task_map_t;

task_map_t tasks[] = {
    { "task_a", 5 },
    { "task_b", 6 },
    { NULL,     0 }
};

int strcmp(const char* str1, const char* str2) {
    const char* p1 = str1;
    const char* p2 = str2;
    for(; *p1 && *p2 && *p1 == *p2; p1++, p2++);
    return *p1 - *p2;
}

int lookup_task_sector(const char* name) {
    for (task_map_t *t = tasks; t->name != NULL; t++) {
        if (strcmp(t->name, name) == 0) {
            return t->sector;
        }
    }
    return -1;
}

int load_task(const char* name, uint32_t* dest) {
    uint32_t sector = lookup_task_sector(name);
    if (sector == -1) {
        return -1;
    }

    int read_count = read_sectors(sector, 1, dest);
    if (read_count == 1) {
        return 0;
    } else {
        return -1;
    }
}

void exec(const char* name) {
    // load
    if (load_task(name, (uint32_t*)TASK_LOAD_ADDR) == 0) {
        // execute
        task_t task = (task_t)TASK_LOAD_ADDR;
        task();
    }
}
