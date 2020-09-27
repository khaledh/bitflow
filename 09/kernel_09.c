#define VIDEO_MEMORY       0xB8000
#define WHITE_ON_LIGHTBLUE 0x9F

void kmain() {
    *((unsigned short *)VIDEO_MEMORY) = (WHITE_ON_LIGHTBLUE << 8) | 'K';

    asm("cli \n"
        "hlt");
}
