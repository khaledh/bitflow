#include "kernel.h"

void task() {
    unsigned short* video_memory = (unsigned short*)VIDEO_MEMORY;
    *(video_memory + 80) = (WHITE_ON_LIGHTBLUE << 8) + 'T';
}
