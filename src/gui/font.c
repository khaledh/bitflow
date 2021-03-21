#include <stdint.h>
#include <device/ata.h>
#include <device/console.h>

#define MZ_SIG ((uint16_t)'Z' << 8 | 'M')
#define NE_SIG ((uint16_t)'E' << 8 | 'N')

#define NE_HEADER_OFFSET 0x3C
#define RES_TABLE_OFFSET 0x24
#define RES_TYPE_FONTDIR 0x8007
#define RES_TYPE_FONT    0x8008

#define read8(buf, start) (*((uint8_t*)&buf[start]))
#define read16(buf, start) (*((uint16_t*)&buf[start]))
#define read32(buf, start) (*((uint32_t*)&buf[start]))

typedef struct res_entry {
    uint16_t file_offset;
    uint16_t length;
    uint16_t flags;
    uint16_t id;
} __attribute__ ((packed)) res_entry_t;

#define MAX_FONT_DIRS 1
#define MAX_FONTS     4

res_entry_t g_res_font_dirs[MAX_FONT_DIRS];
int g_n_res_font_dirs = 0;

res_entry_t g_res_fonts[MAX_FONTS];
int g_n_res_fonts = 0;

//typedef struct glyph_entry_v2 {
//    uint16_t width;
//    uint16_t offset;
//} glyph_entry_v2;
//
typedef struct font_v2 {
    uint16_t version;
    uint32_t size;
    char     copyright[60];
    uint16_t type;
    uint16_t points;
    uint16_t vert_res;
    uint16_t horiz_res;
    uint16_t ascent;
    uint16_t internal_leading;
    uint16_t external_leading;
    uint8_t  italic;
    uint8_t  underline;
    uint8_t  strikeout;
    uint16_t weight;
    uint8_t  charset;
    uint16_t pix_width;
    uint16_t pix_height;
    uint8_t  pitch_and_family;
    uint16_t avg_width;
    uint16_t max_width;
    uint8_t  first_char;
    uint8_t  last_char;
    uint8_t  default_char;
    uint8_t  break_char;
    uint16_t width_bytes;
    uint32_t device;
    uint32_t face;
    uint32_t bits_pointer;
    uint32_t bits_offset;
    uint8_t  reserved;
    struct glyph_entry {
        uint16_t width;
        uint16_t offset;
    } char_table[];
} __attribute__ ((packed)) font_v2_t;

font_v2_t* g_fonts[MAX_FONTS];
int g_n_fonts = 0;

typedef struct font_v3 {
    uint16_t version;
    uint32_t size;
    char     copyright[60];
    uint16_t type;
    uint16_t points;
    uint16_t vert_res;
    uint16_t horiz_res;
    uint16_t ascent;
    uint16_t internal_leading;
    uint16_t external_leading;
    uint8_t  italic;
    uint8_t  underline;
    uint8_t  strikeout;
    uint16_t weight;
    uint8_t  charset;
    uint16_t pix_width;
    uint16_t pix_height;
    uint8_t  pitch_and_family;
    uint16_t avg_width;
    uint16_t max_width;
    uint8_t  first_char;
    uint8_t  last_char;
    uint8_t  default_char;
    uint8_t  break_char;
    uint16_t width_bytes;
    uint32_t device;
    uint32_t face;
    uint32_t bits_pointer;
    uint32_t bits_offset;
    uint8_t  reserved;
    uint32_t flags;
    uint16_t a_space;
    uint16_t b_space;
    uint16_t c_space;
    uint32_t color_pointer;
    uint8_t  reserved1[16];
    struct glyph_entry_v3 {
        uint16_t width;
        uint32_t offset;
    } __attribute__ ((packed)) char_table[];
} __attribute__ ((packed)) font_v3_t;

font_v3_t* g_fonts_v3[MAX_FONTS];
int g_n_fonts_v3 = 0;

void parse_mz_sig(const uint8_t* buf) {
    uint16_t sig = read16(buf, 0);
    if (sig != MZ_SIG) {
        print("Invalid MZ signature.");
    }
}

void parse_ne_sig(const uint8_t* buf) {
    uint16_t sig = read16(buf, 0);
    if (sig != NE_SIG) {
        print("Invalid NE signature.");
    }
}

void parse_resource_table(const uint8_t* buf) {
    uint16_t shift_count = read16(buf, 0);
    buf += 2;

    uint16_t res_type_id = read16(buf, 0);
    while (res_type_id != 0) {
//        printf("Res type:\t\t%#06x %s\n", res_type_id, rt_names[res_type_id & 0x7fff]);
        uint16_t res_count = read16(buf, 2);
//        printf("Res count:\t\t%d\n\n", res_count);
        buf += 8;
        for (int i = 0; i < res_count; i++) {
            res_entry_t* entry;
            if (res_type_id == RES_TYPE_FONTDIR && g_n_res_font_dirs < MAX_FONT_DIRS) {
//                parse_fontdir(&file_buf[res_file_offset]);
                entry = &g_res_font_dirs[g_n_res_font_dirs++];
            }
            else if (res_type_id == RES_TYPE_FONT & g_n_res_fonts < MAX_FONTS) {
//                parse_font(&file_buf[res_file_offset]);
                entry = &g_res_fonts[g_n_res_fonts++];
            }

            entry->file_offset = read16(buf, 0) << shift_count;
            entry->length = read16(buf, 2) << shift_count;
            entry->flags = read16(buf, 4);
            entry->id = read16(buf, 6);

            buf += 12;
        }
        res_type_id = read16(buf, 0);
    }
}

font_v2_t* parse_font(uint8_t* buf) {
    font_v2_t* font = (font_v2_t*)(buf);
    font->bits_pointer = (uint32_t)(buf + font->bits_offset);
    return font;
}

font_v3_t* parse_font_v3(uint8_t* buf) {
    font_v3_t* font = (font_v3_t*)(buf);
    font->bits_pointer = (uint32_t)(buf + font->bits_offset);
    return font;
}

void parse_ne(uint8_t* buf, uint32_t offset) {
    uint8_t* ne_header = &buf[offset];
    parse_ne_sig(ne_header);

    uint16_t resource_table_offset = read16(ne_header, RES_TABLE_OFFSET);
    parse_resource_table(&ne_header[resource_table_offset]);

    for (int i = 0; i < g_n_res_fonts; i++) {
        uint8_t* fontinfo = &buf[g_res_fonts[i].file_offset];
        if (read16(fontinfo, 0) == 0x0200) {
            g_fonts[g_n_fonts++] = parse_font(fontinfo);
        } else {
            g_fonts_v3[g_n_fonts_v3++] = parse_font_v3(fontinfo);
            print("loaded v3 font");
        }
    }
}

/**
 * Font public functions.
 */

void font_load() {
    static uint8_t buf[12848];

    read_sectors(92, 10, (uint32_t*)buf);

    parse_mz_sig(buf);

    uint32_t ne_header_offset = read32(buf, NE_HEADER_OFFSET);
    parse_ne(buf, ne_header_offset);
}

uint8_t* font_get_glyph(uint8_t num) {
//    font_v2_t* font = g_fonts[0];
    font_v3_t* font = g_fonts_v3[0];
    if (num < font->first_char || num > font->last_char) {
        return 0;
    }
//    uint16_t offset = font->char_table[num - font->first_char].offset;
    uint32_t offset = font->char_table[num - font->first_char].offset;
    print("\noffset: ");
    print_hex32((uint32_t)offset);
    return (uint8_t*)font + offset;
}
