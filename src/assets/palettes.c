#include "palettes.h"
#include "dma.h"

/* ============================================================
 *  Palette Data — Gravity Lab color scheme
 *
 *  BG Palette (256 colors = 16 sub-palettes × 16 colors):
 *    Sub-palette 0: Main circuit colors
 *    Sub-palette 1: Text - white
 *    Sub-palette 2: Text - red
 *    Sub-palette 3: Text - green
 *    Sub-palette 4: Text - yellow
 *    Sub-palette 5: Text - cyan
 *    Sub-palette 6: Text - silver/gray
 *    Sub-palette 7: Text - gold
 *    Sub-palette 8-15: Reserved
 *
 *  OBJ Palette (256 colors = 16 sub-palettes × 16 colors):
 *    Sub-palette 0: Ship + shared sprites
 *    Sub-palette 1-15: Reserved for special effects
 * ============================================================ */

/* BG Palette — main circuit colors (sub-palette 0) */
static const u16 bg_pal_main[16] = {
    0x0000,  /* 0:  Black / transparent          BGR(0,0,0) */
    0x7FFF,  /* 1:  White                        BGR(31,31,31) */
    0x4A4A,  /* 2:  Dark gray (metal)             BGR(9,9,9) */
    0x739C,  /* 3:  Medium gray (highlight)       BGR(14,14,12) */
    0x296B,  /* 4:  Blue (circuit floor)           BGR(5,18,5) */
    0x5293,  /* 5:  Light blue (glow)             BGR(10,18,4) */
    0x7C00,  /* 6:  Red (danger/obstacle)          BGR(31,0,0) */
    0x001F,  /* 7:  Deep blue                     BGR(0,0,31) */
    0x03E0,  /* 8:  Green (checkpoint)             BGR(0,31,0) */
    0x7FE0,  /* 9:  Yellow (boost)                 BGR(31,31,0) */
    0x5EBF,  /* 10: Cyan (accent)                 BGR(11,29,15) */
    0x4208,  /* 11: Dark red (wall shadow)         BGR(8,0,2) */
    0x8410,  /* 12: Orange (fire/explosion)        BGR(16,2,0) */
    0x6B4A,  /* 13: Gray-blue (floor accent)      BGR(13,10,2) */
    0x2124,  /* 14: Very dark blue (void)         BGR(0,4,2) */
    0x57E0,  /* 15: Light green (finish)           BGR(10,31,0) */
};

/* Text sub-palette colors — each sub-palette uses index 0=black, 1=color */
static const u16 text_colors[8] = {
    0x7FFF,  /* Sub-pal 1: White    */
    0x7C00,  /* Sub-pal 2: Red      */
    0x03E0,  /* Sub-pal 3: Green    */
    0x7FE0,  /* Sub-pal 4: Yellow   */
    0x5EBF,  /* Sub-pal 5: Cyan     */
    0x739C,  /* Sub-pal 6: Silver   */
    0xFD20,  /* Sub-pal 7: Gold     */
    0x4208,  /* Sub-pal 8: Bronze   */
};

/* OBJ Palette — ship and sprite colors (sub-palette 0) */
static const u16 obj_pal_main[16] = {
    0x0000,  /* 0:  Transparent                */
    0x7FFF,  /* 1:  White (ship body)          */
    0x5EBF,  /* 2:  Cyan (ship glow/outline)   */
    0x7C00,  /* 3:  Red (thrust flame)         */
    0x7FE0,  /* 4:  Yellow (fire/spark)        */
    0x8410,  /* 5:  Orange (flame inner)       */
    0x739C,  /* 6:  Silver (metal parts)       */
    0x4210,  /* 7:  Dark orange (shadow)       */
    0x001F,  /* 8:  Deep blue (cockpit)        */
    0x03E0,  /* 9:  Green (active indicator)    */
    0x7BEF,  /* 10: Light gray (smoke)         */
    0x294A,  /* 11: Dark cyan (boost trail)    */
    0xFD20,  /* 12: Gold (gold medal)          */
    0xC618,  /* 13: Silver medal color          */
    0x8400,  /* 14: Bronze medal color          */
    0x5293,  /* 15: Light blue glow             */
};

/* -------------------------------------------------------
 *  palettes_load — Write all palette data to GBA hardware
 * ------------------------------------------------------- */
void palettes_load(void) {
    /* --- BG Palette --- */

    /* Sub-palette 0: Main circuit colors */
    int i;
    for (i = 0; i < 16; i++) {
        BG_PALETTE[i] = bg_pal_main[i];
    }

    /* Sub-palettes 1-8: Text colors (index 0 = black, index 1 = color) */
    for (i = 0; i < 8; i++) {
        u16 base = (u16)((i + 1) * 16);
        BG_PALETTE[base + 0] = 0x0000;            /* Background: black */
        BG_PALETTE[base + 1] = text_colors[i];     /* Foreground: text color */
    }

    /* Fill remaining BG palette entries with black */
    for (i = 16 * 9; i < 256; i++) {
        BG_PALETTE[i] = 0x0000;
    }

    /* --- OBJ Palette --- */

    /* Sub-palette 0: Main sprite colors */
    for (i = 0; i < 16; i++) {
        OBJ_PALETTE[i] = obj_pal_main[i];
    }

    /* Medal sub-palettes */
    /* Sub-palette 1: Gold medal */
    OBJ_PALETTE[16] = 0x0000;   /* Transparent */
    OBJ_PALETTE[17] = 0xFD20;   /* Gold */
    OBJ_PALETTE[18] = 0x7FE0;   /* Yellow highlight */
    OBJ_PALETTE[19] = 0x8410;   /* Orange shadow */

    /* Sub-palette 2: Silver medal */
    OBJ_PALETTE[32] = 0x0000;
    OBJ_PALETTE[33] = 0xC618;
    OBJ_PALETTE[34] = 0x7FFF;
    OBJ_PALETTE[35] = 0x4A4A;

    /* Sub-palette 3: Bronze medal */
    OBJ_PALETTE[48] = 0x0000;
    OBJ_PALETTE[49] = 0x8400;
    OBJ_PALETTE[50] = 0xC618;
    OBJ_PALETTE[51] = 0x4208;

    /* Fill remaining OBJ palette with black */
    for (i = 64; i < 256; i++) {
        OBJ_PALETTE[i] = 0x0000;
    }
}
