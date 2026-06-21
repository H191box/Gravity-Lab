#include "text_renderer.h"
#include "bg_manager.h"
#include "gba_header.h"
#include "dma.h"
#include "font.h"

/* Current text BG layer */
static int text_layer = BG_LAYER_UI;

/* Font tile data starts at tile 0 in the text BG's charbase.
 * For color text, we need 2 copies of each character tile:
 * one for each color. But that's 2 * 95 * 32 = 6080 bytes.
 *
 * Simpler approach: Use 2-color tiles (1bpp expanded to 4bpp).
 * Palette entry 0 = background, entry 1 = foreground.
 * For different colors, we change which palette the tile uses.
 * But in 16-color mode, each tile uses a 16-color sub-palette.
 * We can define multiple sub-palettes with different colors.
 *
 * For text layer (BG3):
 *   Palette 0: Black text on white bg  (not needed)
 *   Palette 1: White text (pal[1]=white, pal[0]=black)
 *   Palette 2: Red text
 *   Palette 3: Green text
 *   Palette 4: Yellow text
 *   Palette 5: Cyan text
 *   Palette 6: Gray text
 *   Palette 7: Gold text
 *
 * We load font tiles once, and use the palette field in map entries
 * to select the color.
 */

/* -------------------------------------------------------
 *  text_init — Load font into BG charbase and set up text layer
 * ------------------------------------------------------- */
void text_init(void) {
    /* Load font tile data into CharBase 1 (0x06004000)
     * Each character is 8x8, 4bpp = 32 bytes = 16 halfwords
     * We store 2 colors: index 0 = bg (black), index 1 = fg (color)
     * Font data from font.h is 1bpp (8 bytes per char).
     * We expand to 4bpp: each bit becomes a 4-bit nibble.
     */

    u16 *char_base = (u16 *)BG_CHARBASE(1);  /* 0x06004000 */
    u8 printable;  /* Character index: 0 = space (0x20), 1 = '!', etc. */

    for (printable = 0; printable < 95; printable++) {
        const u8 *glyph = font_data[printable];  /* 8 bytes, 1bpp */
        u16 tile[16];  /* 32 bytes = 16 halfwords for one 4bpp 8x8 tile */
        u8 row, bit;

        for (row = 0; row < 8; row++) {
            u8 line = glyph[row];
            u16 upper = 0;  /* Upper 4 pixels of row */
            u16 lower = 0;  /* Lower 4 pixels of row */

            for (bit = 0; bit < 4; bit++) {
                if (line & (0x80 >> bit))
                    upper |= (1 << (bit * 4));  /* Set color index 1 */
                if (line & (0x08 >> bit))
                    lower |= (1 << (bit * 4));  /* Wait, this is wrong */
            }
            /* Actually: for 4bpp, each pixel is a nibble.
             * 8 pixels per row = 4 bytes = 2 u16s.
             * Pixel 0 = bits 0-3 of first byte
             * Pixel 1 = bits 4-7 of first byte
             * Pixel 2 = bits 0-3 of second byte
             * Pixel 3 = bits 4-7 of second byte
             * ...
             * In u16 (little-endian):
             *   Word 0 = pixel3:pixel2:pixel1:pixel0
             *   Word 1 = pixel7:pixel6:pixel5:pixel4
             */

            u16 w0 = 0, w1 = 0;
            for (bit = 0; bit < 8; bit++) {
                u16 color = (line & (0x80 >> bit)) ? 1 : 0;
                if (bit < 4) {
                    w0 |= (color << (bit * 4));
                } else {
                    w1 |= (color << ((bit - 4) * 4));
                }
            }

            tile[row * 2]     = w0;
            tile[row * 2 + 1] = w1;
        }

        /* Copy tile to charbase */
        int j;
        for (j = 0; j < 16; j++) {
            char_base[printable * 16 + j] = tile[j];
        }
    }

    /* Set up text sub-palettes
     * Each sub-palette has 16 colors. For 2-color text, we only use indices 0 and 1.
     * Palette 0 (index 0): Black background, white text
     * Palette 1 (index 1): Black background, red text
     * etc.
     *
     * BG palette is 256 colors = 16 sub-palettes of 16 colors.
     * Sub-palette n starts at color n*16.
     */
    u8 pal;
    u16 colors[8] = {
        0x7FFF,  /* White */
        0x7C00,  /* Red */
        0x03E0,  /* Green */
        0x7FE0,  /* Yellow */
        0x5EBF,  /* Cyan */
        0x739C,  /* Silver/gray */
        0xFD20,  /* Gold */
        0x4208   /* Dark orange / bronze */
    };

    for (pal = 0; pal < 8; pal++) {
        u16 base = pal * 16;
        BG_PALETTE[base + 0] = 0x0000;       /* Background: black (transparent conceptually) */
        BG_PALETTE[base + 1] = colors[pal];   /* Foreground: the color */
    }

    /* Configure text BG layer */
    BGConfig text_cfg = {
        .priority    = 3,
        .char_base   = 1,
        .screen_base = 30,  /* 0x0600F000 */
        .size        = BG_CNT_SCREENDIM_32x32,
        .palette_256 = FALSE,
        .wrap        = FALSE,
        .enabled     = TRUE
    };
    bg_configure(text_layer, &text_cfg);
    bg_clear_map(text_layer);
    bg_scroll(text_layer, 0, 0);
}

/* -------------------------------------------------------
 *  text_clear — Fill tile map with empty tile
 * ------------------------------------------------------- */
void text_clear(void) {
    bg_clear_map(text_layer);
}

/* -------------------------------------------------------
 *  text_draw_char — Draw one character at tile position
 *  Uses the palette field for color selection.
 * ------------------------------------------------------- */
void text_draw_char(u8 x, u8 y, char ch, u8 color) {
    if (x >= 30 || y >= 20) return;  /* Out of bounds */

    /* Convert ASCII to tile index */
    u8 tile_idx = 0;
    if (ch >= 0x20 && ch <= 0x7E) {
        tile_idx = (u8)(ch - 0x20);
    } else {
        tile_idx = 0;  /* Unknown char → space */
    }

    /* Clamp color to valid palette range */
    u8 pal = MIN(color, 15);

    /* Write to tile map */
    bg_set_map_entry(text_layer, x, y, tile_idx, pal, FALSE, FALSE);
}

/* -------------------------------------------------------
 *  text_draw — Draw a string at tile position
 *  Returns number of characters drawn.
 * ------------------------------------------------------- */
int text_draw(u8 x, u8 y, const char *str, u8 color) {
    int count = 0;
    while (*str) {
        text_draw_char(x, y, *str, color);
        x++;
        count++;
        if (x >= 30) break;  /* Stop at screen edge */
        str++;
    }
    return count;
}

/* -------------------------------------------------------
 *  text_draw_int — Draw an integer with zero/space padding
 * ------------------------------------------------------- */
int text_draw_int(u8 x, u8 y, s32 num, u8 digits, u8 color) {
    char buf[12];
    int i = 0;
    int count = 0;
    bool negative = FALSE;
    u32 unum;

    if (num < 0) {
        negative = TRUE;
        unum = (u32)(-num);
    } else {
        unum = (u32)num;
    }

    /* Convert to string (reversed) */
    if (unum == 0) {
        buf[i++] = '0';
    } else {
        while (unum > 0 && i < 11) {
            buf[i++] = (char)('0' + (unum % 10));
            unum /= 10;
        }
    }

    /* Pad with spaces to reach minimum digits */
    while (i < digits) {
        buf[i++] = ' ';
    }

    /* Add negative sign */
    if (negative) {
        buf[i++] = '-';
    }

    /* Reverse and draw */
    int j;
    for (j = i - 1; j >= 0; j--) {
        text_draw_char(x, y, buf[j], color);
        x++;
        count++;
        if (x >= 30) break;
    }

    return count;
}

/* -------------------------------------------------------
 *  text_draw_time — Draw time as MM:SS.CC
 *  centiseconds: total 1/100th seconds
 * ------------------------------------------------------- */
int text_draw_time(u8 x, u8 y, u32 centiseconds, u8 color) {
    u32 total_cs = centiseconds;
    u32 cs = total_cs % 100;
    u32 total_sec = total_cs / 100;
    u32 sec = total_sec % 60;
    u32 min = total_sec / 60;

    int count = 0;

    /* Minutes (2 digits) */
    text_draw_char(x, y, '0' + (min / 10), color); x++; count++;
    text_draw_char(x, y, '0' + (min % 10), color); x++; count++;

    /* Colon */
    text_draw_char(x, y, ':', color); x++; count++;

    /* Seconds (2 digits) */
    text_draw_char(x, y, '0' + (sec / 10), color); x++; count++;
    text_draw_char(x, y, '0' + (sec % 10), color); x++; count++;

    /* Decimal point */
    text_draw_char(x, y, '.', color); x++; count++;

    /* Centiseconds (2 digits) */
    text_draw_char(x, y, '0' + (cs / 10), color); x++; count++;
    text_draw_char(x, y, '0' + (cs % 10), color); x++; count++;

    return count;
}

/* -------------------------------------------------------
 *  text_set_layer — Change target BG layer
 * ------------------------------------------------------- */
void text_set_layer(int layer) {
    if (layer >= 0 && layer <= 3) {
        text_layer = layer;
    }
}