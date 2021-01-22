#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '*' : ' '), \
  (byte & 0x40 ? '*' : ' '), \
  (byte & 0x20 ? '*' : ' '), \
  (byte & 0x10 ? '*' : ' '), \
  (byte & 0x08 ? '*' : ' '), \
  (byte & 0x04 ? '*' : ' '), \
  (byte & 0x02 ? '*' : ' '), \
  (byte & 0x01 ? '*' : ' ')

#define RT_FONTDIR 7
#define RT_FONT    8

#define read8(buf, start) (*((uint8_t*)&buf[start]))
#define read16(buf, start) (*((uint16_t*)&buf[start]))
#define read32(buf, start) (*((uint32_t*)&buf[start]))

char* rt_names[] = {
    [RT_FONTDIR] = "RT_FONTDIR",
    [RT_FONT]    = "RT_FONT",
};

uint8_t file_buf[4960];
uint16_t g_res_file_offset;
uint16_t g_font_version;

_Bool parse_mz_sig(uint8_t* buf) {
    uint16_t sig = read16(buf, 0);
    printf("MZ signature:\t\t%#06x\n", sig);
    if (sig != ((uint16_t)'Z' << 8 | 'M')) {
        printf("Invalid MZ signature.");
        exit(1);
    }
}

_Bool parse_ne_sig(uint8_t* buf) {
    uint16_t sig = read16(buf, 0);
    printf("NE signature:\t\t%#06x\n", sig);
    if (sig != ((uint16_t)'E' << 8 | 'N')) {
        printf("Invalid NE signature.");
        exit(1);
    }
}

typedef struct fontdir_entry {
    uint16_t dfVersion;
    uint32_t dfSize;
    char dfCopyright[60];
    uint16_t dfType;
    uint16_t dfPoints;
    uint16_t dfVertRes;
    uint16_t dfHorizRes;
    uint16_t dfAscent;
    uint16_t dfInternalLeading;
    uint16_t dfExternalLeading;
    uint8_t dfItalic;
    uint8_t dfUnderline;
    uint8_t dfStrikeOut;
    uint16_t dfWeight;
    uint8_t dfCharSet;
    uint16_t dfPixWidth;
    uint16_t dfPixHeight;
    uint8_t dfPitchAndFamily;
    uint16_t dfAvgWidth;
    uint16_t dfMaxWidth;
    uint8_t dfFirstChar;
    uint8_t dfLastChar;
    uint8_t dfDefaultChar;
    uint8_t dfBreakChar;
    uint16_t dfWidthBytes;
    uint32_t dfDevice;
    uint32_t dfFace;
    uint32_t dfReserved;
    char   szDeviceAndFaceName[];
} __attribute__ ((packed)) fontdir_entry_t;

void parse_fontdir(uint8_t* buf) {
    uint16_t n_fonts = read16(buf, 0);
    printf("FontDir num fonts:\t%d\n", n_fonts);

    buf += 2;
    for (int i = 0; i < n_fonts; i++) {
        uint16_t font_ordinal = read16(buf, 0);
        printf("Font ordinal:\t\t%d\n", font_ordinal);
        buf += 2;

        fontdir_entry_t* entry = (fontdir_entry_t*)(buf);
        printf("Font Version:\t\t%#06x\n", entry->dfVersion);
        printf("Font Size:\t\t%d\n", entry->dfSize);
        printf("Font Copyright:\t\t%s\n", entry->dfCopyright);
        printf("Font Type:\t\t%#06x %s\n", entry->dfType, entry->dfType & 0x1 ? "[Vector]" : "[Raster]");
        printf("Font Points:\t\t%d\n", entry->dfPoints);
        printf("Font VertRes:\t\t%d\n", entry->dfVertRes);
        printf("Font HorizRes:\t\t%d\n", entry->dfHorizRes);
        printf("Font Ascent:\t\t%d\n", entry->dfAscent);
        printf("Font InternalLeading:\t%d\n", entry->dfInternalLeading);
        printf("Font ExternalLeading:\t%d\n", entry->dfExternalLeading);
        printf("Font Italic:\t\t%d\n", entry->dfItalic);
        printf("Font Underline:\t\t%d\n", entry->dfUnderline);
        printf("Font StrikeOut:\t\t%d\n", entry->dfStrikeOut);
        printf("Font Weight:\t\t%d\n", entry->dfWeight);
        printf("Font CharSet:\t\t%d\n", entry->dfCharSet);
        printf("Font PixWidth:\t\t%d\n", entry->dfPixWidth);
        printf("Font PixHeight:\t\t%d\n", entry->dfPixHeight);
        printf("Font PitchAndFamily:\t%#04x\n", entry->dfPitchAndFamily);
        printf("Font AvgWidth:\t\t%d\n", entry->dfAvgWidth);
        printf("Font MaxWidth:\t\t%d\n", entry->dfMaxWidth);
        printf("Font FirstChar:\t\t%c [%#04x]\n", entry->dfFirstChar, entry->dfFirstChar);
        printf("Font LastChar:\t\t%c [%#04x]\n", entry->dfLastChar, entry->dfLastChar);
        printf("Font DefaultChar:\t%c [%#04x]\n", entry->dfDefaultChar, entry->dfDefaultChar);
        printf("Font BreakChar:\t\t%c [%#04x]\n", entry->dfBreakChar, entry->dfBreakChar);
        printf("Font WidthBytes:\t%#06x\n", entry->dfWidthBytes);
        printf("Font Device (offset):\t%d\n", entry->dfDevice);
        printf("Font Face (offset):\t%d\n", entry->dfFace);
        printf("Font DeviceName:\t%s\n", &entry->szDeviceAndFaceName[0]);
        printf("Font FaceName:\t\t%s\n", &entry->szDeviceAndFaceName[1]);
    }
}

void print_glyph(uint8_t* buf, char glyph_num[]) {
    printf("Font Bits[%s][00]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][01]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][02]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][03]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][04]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][05]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][06]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][07]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][08]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][09]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][10]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][11]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
    printf("Font Bits[%s][12]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_num, BYTE_TO_BINARY(read8(buf, 0))); buf += 1;
}

void parse_font(uint8_t* buf) {
    uint8_t* fontinfo = buf;

    fontdir_entry_t* entry = (fontdir_entry_t*)(buf);
    printf("Font Version:\t\t%#06x\n", entry->dfVersion);
    printf("Font Size:\t\t%d\n", entry->dfSize);
    printf("Font Copyright:\t\t%s\n", entry->dfCopyright);
    printf("Font Type:\t\t%#06x %s\n", entry->dfType, entry->dfType & 0x1 ? "[Vector]" : "[Raster]");
    printf("Font Points:\t\t%d\n", entry->dfPoints);
    printf("Font VertRes:\t\t%d\n", entry->dfVertRes);
    printf("Font HorizRes:\t\t%d\n", entry->dfHorizRes);
    printf("Font Ascent:\t\t%d\n", entry->dfAscent);
    printf("Font InternalLeading:\t%d\n", entry->dfInternalLeading);
    printf("Font ExternalLeading:\t%d\n", entry->dfExternalLeading);
    printf("Font Italic:\t\t%d\n", entry->dfItalic);
    printf("Font Underline:\t\t%d\n", entry->dfUnderline);
    printf("Font StrikeOut:\t\t%d\n", entry->dfStrikeOut);
    printf("Font Weight:\t\t%d\n", entry->dfWeight);
    printf("Font CharSet:\t\t%d\n", entry->dfCharSet);
    printf("Font PixWidth:\t\t%d\n", entry->dfPixWidth);
    printf("Font PixHeight:\t\t%d\n", entry->dfPixHeight);
    printf("Font PitchAndFamily:\t%#04x\n", entry->dfPitchAndFamily);
    printf("Font AvgWidth:\t\t%d\n", entry->dfAvgWidth);
    printf("Font MaxWidth:\t\t%d\n", entry->dfMaxWidth);
    printf("Font FirstChar:\t\t%c [%#04x]\n", entry->dfFirstChar, entry->dfFirstChar);
    printf("Font LastChar:\t\t%c [%#04x]\n", entry->dfLastChar, entry->dfLastChar);
    printf("Font DefaultChar:\t%c [%#04x]\n", entry->dfDefaultChar, entry->dfDefaultChar);
    printf("Font BreakChar:\t\t%c [%#04x]\n", entry->dfBreakChar, entry->dfBreakChar);
    printf("Font WidthBytes:\t%#06x\n", entry->dfWidthBytes);
    printf("Font Device (offset):\t%d\n", entry->dfDevice);
    printf("Font Face (offset):\t[%d] %s\n", entry->dfFace, &buf[entry->dfFace]);

    buf = (uint8_t*)&entry->dfReserved;

    printf("Font BitsPointer:\t%#010x\n", read32(buf, 0)); buf += 4;
    uint32_t bits_offset = read32(buf, 0);
    printf("Font BitsOffset:\t%#010x\n", read32(buf, 0)); buf += 4;
    buf += 1; // reserved byte
    if (entry->dfVersion == 0x0300) {
        printf("Font Flags:\t\t%#010x\n", read32(buf, 0)); buf += 4;
        printf("Font A space:\t\t%#06x\n", read16(buf, 0)); buf += 2;
        printf("Font B space:\t\t%#06x\n", read16(buf, 0)); buf += 2;
        printf("Font C space:\t\t%#06x\n", read16(buf, 0)); buf += 2;
        printf("Font ColorPointer:\t%#010x\n", read32(buf, 0)); buf += 4;
        buf += 16; // reserved bytes
    }

    printf("Font CharTable[0]:Width:\t%#06x\n", read16(buf, 0)); buf += 2;
    if (entry->dfVersion == 0x0300) {
        printf("Font CharTable[0]:Offset:\t%#010x\n", read32(buf, 0)); buf += 4;
    } else {
        printf("Font CharTable[0]:Offset:\t%#06x\n", read16(buf, 0)); buf += 2;
    }
    printf("Font CharTable[1]:Width:\t%#06x\n", read16(buf, 0)); buf += 2;
    if (entry->dfVersion == 0x0300) {
        printf("Font CharTable[1]:Offset:\t%#010x\n", read32(buf, 0)); buf += 4;
    } else {
        printf("Font CharTable[1]:Offset:\t%#06x\n", read16(buf, 0)); buf += 2;
    }
    printf("...\n");
    buf += (entry->dfLastChar - entry->dfFirstChar - 3) * (entry->dfVersion == 0x0300 ? 6 : 4);
    printf("Font CharTable[254]:Width:\t%#06x\n", read16(buf, 0)); buf += 2;
    if (entry->dfVersion == 0x0300) {
        printf("Font CharTable[254]:Offset:\t%#010x\n", read32(buf, 0)); buf += 4;
    } else {
        printf("Font CharTable[254]:Offset:\t%#06x\n", read16(buf, 0)); buf += 2;
    }
    printf("Font CharTable[255]:Width:\t%#06x\n", read16(buf, 0)); buf += 2;
    if (entry->dfVersion == 0x0300) {
        printf("Font CharTable[255]:Offset:\t%#010x\n", read32(buf, 0)); buf += 4;
    } else {
        printf("Font CharTable[255]:Offset:\t%#06x\n", read16(buf, 0)); buf += 2;
    }
    printf("Font CharTable[___]:Width:\t%#06x\n", read16(buf, 0)); buf += 2;
    if (entry->dfVersion == 0x0300) {
        printf("Font CharTable[___]:Offset:\t%#010x\n", read32(buf, 0)); buf += 4;
    } else {
        printf("Font CharTable[__]:Offset:\t%#06x\n", read16(buf, 0)); buf += 2;
    }

    printf("current offset:\t\t%#010x\n", (uint32_t)(buf - fontinfo));

    print_glyph(buf, "0"); buf += 13;
    printf("...\n");

    buf += ('@' - entry->dfFirstChar - 1) * 13;
    print_glyph(buf, "64"); buf += 13;
    printf("...\n");
    print_glyph(buf, "65"); buf += 13;
    printf("...\n");

    buf += ('a' - 'A' - 1) * 13;
    print_glyph(buf, "97"); buf += 13;
    printf("...\n");

    buf += (254 - 'a' - 1) * 13;
    print_glyph(buf, "254"); buf += 13;
    print_glyph(buf, "255"); buf += 13;
    printf("...\n");

    printf("current fontinfo offset:%d\n", (uint32_t)(buf - fontinfo));
    printf("current file offset:\t%d\n", (uint32_t)(buf - fontinfo + g_res_file_offset));

    print_glyph(buf, "___"); buf += 13;
    printf("\n");

    printf("current file offset:\t%d\n", (uint32_t)(buf - fontinfo + g_res_file_offset));

    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 0));
    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 2));
    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 4));
    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 6));
    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 8));
    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 10));

    printf("Font FaceName:\t%s\n", buf); buf += 12;
    printf("\n");

    printf("current file offset:\t%d\n", (uint32_t)(buf - fontinfo + g_res_file_offset));

//    printf("Padding:\t\t%#06x\n", read16(buf, 0)); buf += 2;
//    printf("Padding:\t\t%#06x\n", read16(buf, 0)); buf += 2;
//    printf("Padding:\t\t%#06x\n", read16(buf, 0)); buf += 2;
//    printf("Padding:\t\t%#06x\n", read16(buf, 0)); buf += 2;
//    printf("Padding:\t\t%#06x\n", read16(buf, 0)); buf += 2;
//    printf("Padding:\t\t%#06x\n", read16(buf, 0)); buf += 2;
//    printf("Padding:\t\t%#04x\n", read8(buf, 0)); buf += 1;
//    printf("\n");
//
//    printf("current file offset:\t%d\n", (uint32_t)(buf - fontinfo + g_res_file_offset));
}

void parse_resource_table(uint8_t* buf) {
    uint16_t shift_count = read16(buf, 0);
    printf("Shift count:\t\t%#06x\n", shift_count);

    buf += 2;
    int res_type_count = 0;
    uint16_t res_type_id = read16(buf, 0);
    while (res_type_id != 0) {
        printf("\n");
        res_type_count++;

        printf("Res type:\t\t%#06x %s\n", res_type_id, rt_names[res_type_id & 0x7fff]);
        uint16_t res_count = read16(buf, 2);
        printf("Res count:\t\t%d\n\n", res_count);
        buf += 8;
        for (int i = 0; i < res_count; i++) {
            uint16_t res_file_offset = g_res_file_offset = read16(buf, 0) << shift_count;
            uint16_t res_length = read16(buf, 2) << shift_count;
            uint16_t res_flags = read16(buf, 4);
            uint16_t res_id = read16(buf, 6);

            printf("Res:\t\t\tfile_offset: %#06x length: %#06x flags: %#06x id: %#06x\n", res_file_offset, res_length, res_flags, res_id);

            if (res_type_id == 0x8007) { // fontdir
                parse_fontdir(&file_buf[res_file_offset]);
            }
            else if (res_type_id == 0x8008) { // font
                parse_font(&file_buf[res_file_offset]);
            }
            buf += 12;
        }
        res_type_id = read16(buf, 0);
    }
//    printf("\nRes type count:\t\t%d\n", res_type_count);
}

_Bool parse_ne(uint8_t* buf) {
    parse_ne_sig(buf);

    uint16_t resource_table_offset = read16(buf, 0x24);
    printf("Res table offset:\t%#010x\n", resource_table_offset);

    parse_resource_table(&buf[resource_table_offset]);
}

int main(int argc, char* argv[]) {
    FILE* file = fopen(argv[1], "rb");
    size_t n = fread(file_buf, 4960, 1, file);

    parse_mz_sig(file_buf);

    uint32_t ne_header_offset = read32(file_buf, 0x3C);
    printf("NE header offset:\t%#010x\n", ne_header_offset);

    parse_ne(&file_buf[ne_header_offset]);

    fclose(file);
    return 0;
}
