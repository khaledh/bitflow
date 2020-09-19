#include <stddef.h>
#include <stdint.h>
#include "kernel.h"

#define TASK_LOAD_ADDR 0xa000
typedef void (*task_t)(void);

void clear_screen();
void load_task(uint8_t* dest);

void kmain() {
    clear_screen();

    unsigned short* video_memory = (unsigned short*)VIDEO_MEMORY;
    *(video_memory) = (WHITE_ON_LIGHTBLUE << 8) + 'K';

    load_task((uint8_t*)TASK_LOAD_ADDR);
    task_t task = (task_t)TASK_LOAD_ADDR;
    task();

    asm("cli \n"
        "hlt");
}

/**
 * Screen clearing routine.
 */

void clear_screen() {
    unsigned short* video_memory = (unsigned short*)VIDEO_MEMORY;
    for (int i = 0; i < (80 * 25); i++) {
        *(video_memory + i) = 0;
    }
}

/**
 * Task loading routine.
 */

// #define TASK_SIZE 512
// typedef uint8_t TASK_BLOCK[TASK_SIZE];

// extern TASK_BLOCK __tasks_start;
// TASK_BLOCK* tasks_base = &__tasks_start;

// void load_task(int task_index, uint8_t* dst) {
//     uint8_t* src = (uint8_t*)(tasks_base + task_index);
//     for (int i = 0; i < TASK_SIZE; i++) {
//         dst[i] = src[i];
//     }
// }

#define TASK_SIZE 512

extern uint8_t __tasks_start;
uint8_t* task_base = &__tasks_start;

void load_task(uint8_t* dest) {
    for (int i = 0; i < TASK_SIZE; i++) {
        dest[i] = task_base[i];
    }
}
