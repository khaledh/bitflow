#define VIDEO_MEMORY       0xB8000
#define LIGHTGRAY_ON_BLACK 0x07

void kmain() {
    *((unsigned short *)VIDEO_MEMORY) = (LIGHTGRAY_ON_BLACK << 8) + 'K';
    for(;;);
}
