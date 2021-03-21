#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MZ_SIG ((uint16_t)'Z' << 8 | 'M')
#define NE_SIG ((uint16_t)'E' << 8 | 'N')
#define PE_SIG ((uint32_t)'E' << 8 | 'P')

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

#define FILE_SIZE 1000000

uint8_t file_buf[FILE_SIZE];
uint16_t g_res_file_offset;
uint16_t g_font_version;

void parse_mz_sig(uint8_t* buf) {
    uint16_t sig = read16(buf, 0);
    printf("MZ signature:\t\t%#06x\n", sig);
    if (sig != ((uint16_t)'Z' << 8 | 'M')) {
        printf("Invalid MZ signature.");
        exit(1);
    }
}

//void parse_ne_sig(uint8_t* buf) {
//    uint16_t sig = read16(buf, 0);
//    printf("NE signature:\t\t%#06x\n", sig);
//    if (sig != ((uint16_t)'E' << 8 | 'N')) {
//        printf("Invalid NE signature.");
//        exit(1);
//    }
//}

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
    printf("FontDir num fonts:\t%d\n\n", n_fonts);

    buf += 2;
    for (int i = 0; i < n_fonts; i++) {
        uint16_t font_ordinal = read16(buf, 0);
        printf("Font ordinal:\t\t%d\n\n", font_ordinal);
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

void print_glyph(uint8_t* buf, int glyph_code, int height) {
    for (int i=0; i<height; i++) {
        printf("Font Bits[%d]:\t"BYTE_TO_BINARY_PATTERN"\n", glyph_code, BYTE_TO_BINARY(read8(buf, 0)));
        buf += 1;
    }
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

    uint32_t glyph_offsets[256];
    for (int i=entry->dfFirstChar; i <= entry->dfLastChar; i++) {
        uint16_t glyph_width = read16(buf, 0);
        glyph_offsets[i] = (entry->dfVersion == 0x0300) ? read32(buf, 2) : read16(buf, 2);
        buf += (entry->dfVersion == 0x0300) ? 6 : 4;

        if (i == entry->dfFirstChar || i == (entry->dfFirstChar + 1) || i == (entry->dfLastChar - 1) || i == entry->dfLastChar) {
            printf("Font CharTable[%d]:Width:\t%#06x\n", i, glyph_width);
            printf("Font CharTable[%d]:Offset:\t%#010x\n", i, glyph_offsets[i]);
        }
    }

    printf("Font CharTable[___]:Width:\t%#06x\n", read16(buf, 0)); buf += 2;
    if (entry->dfVersion == 0x0300) {
        printf("Font CharTable[___]:Offset:\t%#010x\n", read32(buf, 0)); buf += 4;
    } else {
        printf("Font CharTable[__]:Offset:\t%#06x\n", read16(buf, 0)); buf += 2;
    }

    printf("offset:\t\t%#010x\n", (uint32_t)(buf - fontinfo));

    for (int i=entry->dfFirstChar; i <= entry->dfLastChar; i++) {
        if (i == entry->dfFirstChar || i == (entry->dfFirstChar + 1) ||
            i == '@' || i == 'A' || i == 'a' ||
            i == (entry->dfLastChar - 1) || i == entry->dfLastChar) {
            printf("glyph offset: %#06x\n", glyph_offsets[i]);
            print_glyph(&fontinfo[glyph_offsets[i]], i, entry->dfPixHeight);
            printf("\n");
        }
    }
    buf = (uint8_t*)(fontinfo + glyph_offsets[entry->dfLastChar] + entry->dfPixHeight);

    printf("current fontinfo offset:%d\n", (uint32_t)(buf - fontinfo));

    print_glyph(buf, -1, entry->dfPixHeight); buf += entry->dfPixHeight;
    printf("\n");

    printf("current file offset:\t%d\n", (uint32_t)(buf - fontinfo + g_res_file_offset));

//    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 0));
//    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 2));
//    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 4));
//    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 6));
//    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 8));
//    printf("Font FaceName Bytes:\t%#06x\n", read16(buf, 10));

    printf("Font FaceName:\t\t%s\n", buf); buf += 12;
    printf("\n");

    printf("current file offset:\t%d\n", (uint32_t)(buf - fontinfo + g_res_file_offset));
}

void parse_resource_table(uint8_t* buf) {
    uint8_t* res_table_buf = buf;

    uint16_t shift_count = read16(buf, 0);
    printf("\n--- Resource Table ----------------------\n");
    printf("Shift count:\t\t%#06x\n", shift_count);

    buf += 2;
    int res_type_count = 0;
    uint16_t res_type_id = read16(buf, 0);
    while (res_type_id != 0) {
        printf("\n");
        res_type_count++;

        printf("-----------------------------------------\n");
        if (res_type_id & 0x8000){
            printf("Res type:\t\t%#06x %s\n", res_type_id, rt_names[res_type_id & 0x7fff]);
        } else {
            uint8_t type_string_len = res_table_buf[res_type_id];
            printf("Res type:\t\t");
            for (int k=0; k<type_string_len; k++) {
                printf("%c", res_table_buf[res_type_id + 1 + k]);
            }
            printf("\n");

            res_type_id += type_string_len + 1;
            type_string_len = res_table_buf[res_type_id];
            printf("Res name:\t\t");
            for (int k=0; k<type_string_len; k++) {
                printf("%c", res_table_buf[res_type_id + 1 + k]);
            }
            printf("\n");
//            printf("Next byte:\t\t%02x\n", res_table_buf[res_type_id + 1 + type_string_len]);
        }
        uint16_t res_count = read16(buf, 2);
        printf("Res count:\t\t%d\n\n", res_count);
        buf += 8;
        for (int i = 0; i < res_count; i++) {
            uint16_t res_file_offset = g_res_file_offset = read16(buf, 0) << shift_count;
            uint16_t res_length = read16(buf, 2) << shift_count;
            uint16_t res_flags = read16(buf, 4);
            uint16_t res_id = read16(buf, 6);

            printf("Res[%d]:\t\t\tfile_offset: %#06x, length: %#06x, flags: %#06x, id: %#06x\n\n",
                   i, res_file_offset, res_length, res_flags, res_id);

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
    printf("-----------------------------------------\n");
    printf("Total res type count:\t%d\n\n", res_type_count);
}

void parse_ne(uint8_t* buf) {
    uint16_t entry_table_offset = read16(buf, 0x04);
    printf("Entry table offset:\t%#010x\n", entry_table_offset);
    uint16_t entry_table_size = read16(buf, 0x06);
    printf("Entry table size:\t%#010x\n", entry_table_size);

    printf("Entries in Seg Table:\t%#06x\n", read16(buf, 0x1C));
    printf("Entries in ModRef Table:%#06x\n", read16(buf, 0x1E));
    printf("Entries in NR Name Table:%#06x\n", read16(buf, 0x20));
    printf("Entries in Res Table:\t%#06x\n", read16(buf, 0x34));


    uint16_t resource_table_offset = read16(buf, 0x24);
    printf("Res table offset:\t%#010x\n", resource_table_offset);

    parse_resource_table(&buf[resource_table_offset]);

    printf("-----------------------------------------\n");

    uint16_t name_table_offset = read16(buf, 0x26);
    printf("Name table offset:\t%#010x\n", name_table_offset);

    uint8_t* name_table = &buf[name_table_offset];
    uint8_t name_string_len = read8(name_table, 0);
    uint8_t* pch = name_table + 1;
    while (name_string_len) {
        printf("String name:\t\t");
        for (int k = 0; k < name_string_len; k++) {
            printf("%c", *pch++);
        }
        printf("\n");

        name_string_len = read8(pch, 0);
        pch++;
    }

    printf("-----------------------------------------\n");

    name_table_offset = read16(buf, 0x2C);
    printf("NR Name table offset:\t%#010x\n", name_table_offset);

    name_table = &buf[name_table_offset];
    name_string_len = read8(name_table, 0);
    printf("Name len: %d\n", name_string_len);
    pch = name_table + 1;
    while (name_string_len) {
        printf("String name:\t\t");
        for (int k = 0; k < name_string_len; k++) {
            printf("%c", *pch++);
        }
        printf("\n");

        name_string_len = read8(pch, 0);
        pch++;
    }
}

void parse_pe(uint8_t* buf) {
    buf += 4; // skip PE signature

    printf("[PE] Machine:\t\t%#06x\n", read16(buf, 0)); buf += 2;
    uint16_t num_sections = read16(buf, 0);
    printf("[PE] NumberOfSections:\t%#06x\n", num_sections); buf += 2;
    buf += 12; // skip some fields
    printf("[PE] SizeOfOptionalHeader:%#06x\n", read16(buf, 0)); buf += 2;
    printf("[PE] Characteristics:\t%#06x\n", read16(buf, 0)); buf += 2;

    // optional header
    uint8_t* opt_hdr = buf;
    uint16_t magic = read16(buf, 0);
    printf("[PE/OH] Magic Number:\t%#06x [%s]\n", magic, magic == 0x10b ? "PE32" : "PE32+"); buf += 2;
    printf("[PE/OH] MajorLinkerVersion:%#04x\n", read8(buf, 0)); buf += 1;
    printf("[PE/OH] MinorLinkerVersion:%#04x\n", read8(buf, 0)); buf += 1;
    printf("[PE/OH] SizeOfCode:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] SizeOfInitializedData:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] SizeOfUninitializedData:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] AddressOfEntryPoint:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] BaseOfCode:\t%#010x\n", read32(buf, 0)); buf += 4;
    if (magic == 0x10b) { // PE32
        printf("[PE/OH] BaseOfData:\t%#010x\n", read32(buf, 0)); buf += 4;
    }
    printf("[PE/OH] ImageBase:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] SectionAlignment:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] FileAlignment:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] MajorOperatingSystemVersion:\t%#010x\n", read16(buf, 0)); buf += 2;
    printf("[PE/OH] MinorOperatingSystemVersion:\t%#010x\n", read16(buf, 0)); buf += 2;
    printf("[PE/OH] MajorImageVersion:\t%#010x\n", read16(buf, 0)); buf += 2;
    printf("[PE/OH] MinorImageVersion:\t%#010x\n", read16(buf, 0)); buf += 2;
    printf("[PE/OH] MajorSubsystemVersion:\t%#010x\n", read16(buf, 0)); buf += 2;
    printf("[PE/OH] MinorSubsystemVersion:\t%#010x\n", read16(buf, 0)); buf += 2;
    printf("[PE/OH] Win32VersionValue:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] SizeOfImage:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] SizeOfHeaders:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] CheckSum:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Subsystem:\t%#010x\n", read16(buf, 0)); buf += 2;
    printf("[PE/OH] DllCharacteristics:\t%#010x\n", read16(buf, 0)); buf += 2;
    printf("[PE/OH] SizeOfStackReserve:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] SizeOfStackCommit:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] SizeOfHeapReserve:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] SizeOfHeapCommit:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] LoaderFlags:\t%#010x\n", read32(buf, 0)); buf += 4;
    uint32_t num_rva_and_sizes = read32(buf, 0);
    printf("[PE/OH] NumberOfRvaAndSizes:\t%#010x\n", num_rva_and_sizes); buf += 4;

    printf("[PE/OH] Export Table RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Export Table Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Import Table RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Import Table Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Resource Table RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Resource Table Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Exception Table RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Exception Table Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Certificate Table RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Certificate Table Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Base Relocation Table RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Base Relocation Table Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Debug RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Debug Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Architecture RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Architecture Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Global Ptr RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Global Ptr Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] TLS Table RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] TLS Table Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Load Config Table RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Load Config Table Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Bound Import RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Bound Import Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] IAT RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] IAT Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Delay Import Descriptor RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Delay Import Descriptor Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] CLR Runtime Header RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] CLR Runtime Header Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("[PE/OH] Reserved RVA:\t%#010x\n", read32(buf, 0)); buf += 4;
    printf("[PE/OH] Reserved Size:\t%#010x\n", read32(buf, 0)); buf += 4;

    printf("Section Headers\n");
    for (int i=0; i<num_sections; i++) {
        printf("[section%d] Name:\t%s\n", i, buf + (40*i));
    }

    uint8_t* rsrc_sec_hdr = buf + 40*5;
    printf("[sec] rsrc Name:\t%s\n", rsrc_sec_hdr); rsrc_sec_hdr += 8;
    printf("[sec] rsrc VirtualSize:\t%#010x\n", read32(rsrc_sec_hdr, 0)); rsrc_sec_hdr += 4;
    uint32_t rsrc_va = read32(rsrc_sec_hdr, 0);
    printf("[sec] rsrc VirtualAddress:\t%#010x\n", rsrc_va); rsrc_sec_hdr += 4;
    printf("[sec] rsrc SizeOfRawData:\t%#010x\n", read32(rsrc_sec_hdr, 0)); rsrc_sec_hdr += 4;
    uint32_t raw_data_ptr = read32(rsrc_sec_hdr, 0);
    printf("[sec] rsrc PointerToRawData:\t%#010x\n", raw_data_ptr); rsrc_sec_hdr += 4;

    //  parse resource directory table
    uint8_t* res_sec_ptr = file_buf + raw_data_ptr;
    uint8_t* res_sec = res_sec_ptr;
    printf("1st Level Dir:\n");
    printf("[rtbl] Characteristics:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Time/Date Stamp:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Major Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] Minor Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # Named Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # ID Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;

    printf("Dir entries:\n");
    printf("[rtbl] Type ID:\t\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Subdir Offset:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Type ID:\t\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    uint32_t subdir_offset = read32(res_sec_ptr, 0);
    printf("[rtbl] Subdir Offset:\t%#010x\n", subdir_offset); res_sec_ptr += 4;

//    res_sec_ptr = file_buf + raw_data_ptr + subdir_offset;
    printf("2nd Level Dir:\n");
    printf("[rtbl] Characteristics:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Time/Date Stamp:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Major Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] Minor Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # Named Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # ID Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;

    printf("Dir entries:\n");
    printf("[rtbl] Name ID:\t\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Subdir Offset:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;

    printf("3rd Level Dir:\n");
    printf("[rtbl] Characteristics:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Time/Date Stamp:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Major Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] Minor Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # Named Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # ID Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;

    printf("Dir entries:\n");
    printf("[rtbl] Language ID:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Entry Offset:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;

    printf("2nd Level Dir:\n");
    printf("[rtbl] Characteristics:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Time/Date Stamp:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Major Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] Minor Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # Named Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    uint16_t id_entries = read16(res_sec_ptr, 0);
    printf("[rtbl] # ID Entries:\t%#06x\n", id_entries); res_sec_ptr += 2;

    printf("Dir entries:\n");
    for (int i=0; i<id_entries; i++) {
        printf("[rtbl] Name ID:\t\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
        printf("[rtbl] Subdir Offset:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    }

    printf("3rd Level Dir:\n");
    printf("[rtbl] Characteristics:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Time/Date Stamp:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Major Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] Minor Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # Named Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # ID Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("Dir entries:\n");
    printf("[rtbl] Language ID:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    uint32_t entry_offset = read32(res_sec_ptr, 0);
    printf("[rtbl] Entry Offset:\t%#010x\n", entry_offset); res_sec_ptr += 4;

    printf("3rd Level Dir:\n");
    printf("[rtbl] Characteristics:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Time/Date Stamp:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Major Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] Minor Version:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # Named Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("[rtbl] # ID Entries:\t%#06x\n", read16(res_sec_ptr, 0)); res_sec_ptr += 2;
    printf("Dir entries:\n");
    printf("[rtbl] Language ID:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[rtbl] Entry Offset:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;

    res_sec_ptr = res_sec + entry_offset;
    uint32_t data_rva = read32(res_sec_ptr, 0);
    uint32_t data_start = data_rva - rsrc_va;
    printf("[entry] Data RVA:\t%#010x\n", data_rva); res_sec_ptr += 4;
    printf("[entry] Size:\t\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] CodePage:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] Reserved:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] Data Start:\t%#010x\n", data_start);

    data_rva = read32(res_sec_ptr, 0);
    data_start = data_rva - rsrc_va;
    printf("[entry] Data RVA:\t%#010x\n", data_rva); res_sec_ptr += 4;
    printf("[entry] Size:\t\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] CodePage:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] Reserved:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] Data Start:\t%#010x\n", data_start);

    data_rva = read32(res_sec_ptr, 0);
    data_start = data_rva - rsrc_va;
    printf("[entry] Data RVA:\t%#010x\n", data_rva); res_sec_ptr += 4;
    printf("[entry] Size:\t\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] CodePage:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] Reserved:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] Data Start:\t%#010x\n", data_start);

    data_rva = read32(res_sec_ptr, 0);
    data_start = data_rva - rsrc_va;
    printf("[entry] Data RVA:\t%#010x\n", data_rva); res_sec_ptr += 4;
    printf("[entry] Size:\t\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] CodePage:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] Reserved:\t%#010x\n", read32(res_sec_ptr, 0)); res_sec_ptr += 4;
    printf("[entry] Data Start:\t%#010x\n", data_start);

    parse_font(res_sec + data_start);
}

int main(int argc, char* argv[]) {
    FILE* file = fopen(argv[1], "rb");
    size_t n = fread(file_buf, FILE_SIZE, 1, file);

    parse_mz_sig(file_buf);

    uint32_t header_offset = read32(file_buf, 0x3C);

    if (read16(file_buf, header_offset) == NE_SIG) {
        printf("NE header offset:\t%#010x\n", header_offset);
        parse_ne(file_buf + header_offset);
    }
    else if (read32(file_buf, header_offset) == PE_SIG) {
        printf("PE header offset:\t%#010x\n", header_offset);
        parse_pe(file_buf + header_offset);
    }

    fclose(file);
    return 0;
}
