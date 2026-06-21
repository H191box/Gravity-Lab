#include "bg_manager.h"
#include "gba_header.h"
#include "dma.h"

/* Current configuration for each layer */
static BGConfig bg_configs[4] = {0};

/* Current scroll positions */
static s16 bg_scroll_x[4] = {0};
static s16 bg_scroll_y[4] = {0};

/* Layer enable mask in DISPCNT */
static const u16 bg_enable_mask[4] = {
    DCNT_BG0, DCNT_BG1, DCNT_BG2, DCNT_BG3
};

/* -------------------------------------------------------
 *  bg_init — Set up default layer configurations
 *
 *  VRAM Layout:
 *    CharBase 0 (0x06000000, 16KB): BG0 tiles (floor)
 *    CharBase 1 (0x06004000, 16KB): BG1 tiles (walls/obstacles)
 *    CharBase 2 (0x06008000, 16KB): BG2 tiles (decorations)
 *    CharBase 3 (0x0600C000, 16KB): BG3 tiles (UI)
 *
 *    ScreenBase 0 (0x06008000): BG0 map (2KB) -- overlaps CharBase2 for BG2
 *    ScreenBase 1 (0x06008800): BG1 map (2KB)
 *    ScreenBase 2 (0x06009000): BG2 map (2KB)
 *    ScreenBase 3 (0x06009800): BG3 map (2KB)
 *
 *  Wait, we need to be more careful with VRAM layout.
 *  Let's use:
 *    CharBase 0 (0x06000000): Shared tile data for all BGs
 *    CharBase 1 (0x06004000): Additional tile data
 *    ScreenBase 8 (0x06010000): BG0 map (up to 8KB for 64x64)
 *    ScreenBase 12 (0x06018000): BG1 map
 *    ScreenBase 16 (0x06020000): BG2 map
 *    ScreenBase 20 (0x06028000): BG3 map
 *
 *  But VRAM is only 96KB = 0x18000. Let me recalculate:
 *  CharBase n = 0x06000000 + n * 0x4000 (16KB per block)
 *  ScreenBase n = 0x06000000 + n * 0x0800 (2KB per block)
 *
 *  Better layout:
 *    CharBase 0 (0x06000000): Tiles for BG0 (floor) - 16KB, 512 tiles
 *    CharBase 1 (0x06004000): Tiles for BG1 (walls) - 16KB, 512 tiles
 *    CharBase 2 (0x06008000): Tiles for BG2 (deco) - 16KB, 512 tiles
 *    CharBase 3 (0x0600C000): Tiles for BG3 (UI) - 16KB, 512 tiles
 *    ScreenBase 24 (0x0600C000): BG0 map — OVERLAPS!
 *
 *  OK, we need to share VRAM. Let's use:
 *    CharBase 0 (0x06000000): ALL tile data (32KB = 0x06000000-0x06007FFF)
 *    ScreenBase 16 (0x06008000): BG0 map (2KB)
 *    ScreenBase 17 (0x06008800): BG1 map (2KB)
 *    ScreenBase 18 (0x06009000): BG2 map (2KB)
 *    ScreenBase 19 (0x06009800): BG3 map (2KB)
 *    Remaining: 0x0600A000-0x06017FFF for more maps/tiles
 *
 *  CharBase uses blocks of 16KB, ScreenBase uses blocks of 2KB.
 *  If all BGs use CharBase 0, they share the same tiles.
 * ------------------------------------------------------- */
void bg_init(void) {
    /* BG0: Circuit floor — 64x64 map, priority 0 */
    BGConfig cfg0 = {
        .priority   = 0,
        .char_base  = 0,
        .screen_base = 16,  /* 0x06008000 */
        .size       = BG_CNT_SCREENDIM_64x64,
        .palette_256 = FALSE,
        .wrap       = TRUE,
        .enabled    = TRUE
    };
    bg_configure(BG_LAYER_FLOOR, &cfg0);

    /* BG1: Walls/obstacles — 64x64 map, priority 1 */
    BGConfig cfg1 = {
        .priority   = 1,
        .char_base  = 0,  /* Share tiles with floor */
        .screen_base = 24,  /* 0x0600C000 */
        .size       = BG_CNT_SCREENDIM_64x64,
        .palette_256 = FALSE,
        .wrap       = TRUE,
        .enabled    = TRUE
    };
    bg_configure(BG_LAYER_WALLS, &cfg1);

    /* BG2: Decorative elements — 32x32 map, priority 2 */
    BGConfig cfg2 = {
        .priority   = 2,
        .char_base  = 0,
        .screen_base = 28,  /* 0x0600E000 */
        .size       = BG_CNT_SCREENDIM_32x32,
        .palette_256 = FALSE,
        .wrap       = FALSE,
        .enabled    = FALSE  /* Start disabled, enable when needed */
    };
    bg_configure(BG_LAYER_DECO, &cfg2);

    /* BG3: UI text layer — 32x32 map, priority 3 */
    BGConfig cfg3 = {
        .priority   = 3,
        .char_base  = 1,  /* Separate tile set for UI */
        .screen_base = 30,  /* 0x0600F000 */
        .size       = BG_CNT_SCREENDIM_32x32,
        .palette_256 = FALSE,
        .wrap       = FALSE,
        .enabled    = FALSE  /* Enable when UI is needed */
    };
    bg_configure(BG_LAYER_UI, &cfg3);

    /* Clear all maps */
    bg_clear_map(BG_LAYER_FLOOR);
    bg_clear_map(BG_LAYER_WALLS);
    bg_clear_map(BG_LAYER_DECO);
    bg_clear_map(BG_LAYER_UI);
}

/* -------------------------------------------------------
 *  bg_configure — Apply a BGConfig to a layer's control register
 * ------------------------------------------------------- */
void bg_configure(int layer, const BGConfig *cfg) {
    if (layer < 0 || layer > 3 || !cfg) return;

    /* Store config */
    bg_configs[layer] = *cfg;

    /* Build control register value */
    u16 cnt = 0;
    cnt |= BG_CNT_PRIORITY(cfg->priority);
    cnt |= BG_CNT_CHARBASE(cfg->char_base);
    cnt |= BG_CNT_SCREENBASE(cfg->screen_base);
    cnt |= cfg->size;

    if (cfg->palette_256) cnt |= BG_CNT_256COLOR;
    if (cfg->wrap)         cnt |= BG_CNT_WRAP;

    /* Write to the appropriate BG control register */
    switch (layer) {
        case 0: REG_BG0CNT = cnt; break;
        case 1: REG_BG1CNT = cnt; break;
        case 2: REG_BG2CNT = cnt; break;
        case 3: REG_BG3CNT = cnt; break;
    }

    /* Enable/disable in DISPCNT */
    if (cfg->enabled) {
        REG_DISPCNT |= bg_enable_mask[layer];
    } else {
        REG_DISPCNT &= ~bg_enable_mask[layer];
    }

    /* Set scroll to 0 */
    bg_scroll(layer, 0, 0);
}

/* -------------------------------------------------------
 *  bg_set_priority — Change layer priority
 * ------------------------------------------------------- */
void bg_set_priority(int layer, u8 priority) {
    if (layer < 0 || layer > 3) return;
    bg_configs[layer].priority = priority;

    /* Re-read control register, modify priority bits, write back */
    volatile u16 *cnt_reg;
    switch (layer) {
        case 0: cnt_reg = &REG_BG0CNT; break;
        case 1: cnt_reg = &REG_BG1CNT; break;
        case 2: cnt_reg = &REG_BG2CNT; break;
        case 3: cnt_reg = &REG_BG3CNT; break;
        default: return;
    }

    u16 cnt = *cnt_reg;
    cnt &= ~BG_CNT_PRIORITY(3);     /* Clear priority bits */
    cnt |= BG_CNT_PRIORITY(priority); /* Set new priority */
    *cnt_reg = cnt;
}

/* -------------------------------------------------------
 *  bg_scroll — Set scroll offset for a BG layer
 *  Only lower 9 bits are used by hardware (0-511).
 * ------------------------------------------------------- */
void bg_scroll(int layer, s16 x, s16 y) {
    if (layer < 0 || layer > 3) return;

    /* Store scroll (may be larger than 511 for world coords) */
    bg_scroll_x[layer] = x;
    bg_scroll_y[layer] = y;

    /* Apply to hardware registers (only 9 bits used) */
    volatile u16 *hofs_reg;
    volatile u16 *vofs_reg;
    switch (layer) {
        case 0: hofs_reg = &REG_BG0HOFS; vofs_reg = &REG_BG0VOFS; break;
        case 1: hofs_reg = &REG_BG1HOFS; vofs_reg = &REG_BG1VOFS; break;
        case 2: hofs_reg = &REG_BG2HOFS; vofs_reg = &REG_BG2VOFS; break;
        case 3: hofs_reg = &REG_BG3HOFS; vofs_reg = &REG_BG3VOFS; break;
        default: return;
    }

    *hofs_reg = (u16)(x & 0x1FF);
    *vofs_reg = (u16)(y & 0x1FF);
}

/* -------------------------------------------------------
 *  bg_get_scroll — Read current scroll position
 * ------------------------------------------------------- */
void bg_get_scroll(int layer, s16 *x, s16 *y) {
    if (layer < 0 || layer > 3) return;
    if (x) *x = bg_scroll_x[layer];
    if (y) *y = bg_scroll_y[layer];
}

/* -------------------------------------------------------
 *  bg_enable / bg_disable — Toggle layer in DISPCNT
 * ------------------------------------------------------- */
void bg_enable(int layer) {
    if (layer < 0 || layer > 3) return;
    bg_configs[layer].enabled = TRUE;
    REG_DISPCNT |= bg_enable_mask[layer];
}

void bg_disable(int layer) {
    if (layer < 0 || layer > 3) return;
    bg_configs[layer].enabled = FALSE;
    REG_DISPCNT &= ~bg_enable_mask[layer];
}

/* -------------------------------------------------------
 *  bg_get_map_address — Get VRAM address of a layer's tile map
 *  Map address = 0x06000000 + screenbase * 0x0800
 * ------------------------------------------------------- */
u16 *bg_get_map_address(int layer) {
    if (layer < 0 || layer > 3) return (u16 *)VRAM_BASE;
    u8 sb = bg_configs[layer].screen_base;
    return (u16 *)(VRAM_BASE + (u32)sb * 0x0800);
}

/* -------------------------------------------------------
 *  bg_get_char_address — Get VRAM address of a layer's character data
 *  Char address = 0x06000000 + charbase * 0x4000
 * ------------------------------------------------------- */
u16 *bg_get_char_address(int layer) {
    if (layer < 0 || layer > 3) return (u16 *)VRAM_BASE;
    u8 cb = bg_configs[layer].char_base;
    return (u16 *)(VRAM_BASE + (u32)cb * 0x4000);
}

/* -------------------------------------------------------
 *  bg_set_map_entry — Write a single tile to a BG map
 *
 *  16-color map entry format:
 *    Bits 0-9:   Tile index (0-1023)
 *    Bits 10-11: Horizontal flip, Vertical flip
 *    Bits 12-15: Palette number (0-15)
 * ------------------------------------------------------- */
void bg_set_map_entry(int layer, u8 col, u8 row, u16 tile_id,
                       u8 palette, bool hflip, bool vflip) {
    if (layer < 0 || layer > 3) return;

    u16 *map = bg_get_map_address(layer);

    /* For 64x64 maps, the map is split into 4 32x32 screens
     * Screen layout in memory:
     *   [0]: col 0-31, row 0-31
     *   [1]: col 32-63, row 0-31
     *   [2]: col 0-31, row 32-63
     *   [3]: col 32-63, row 32-63
     */
    u8 screen_col = col >> 5;  /* 0 or 1 */
    u8 screen_row = row >> 5;  /* 0 or 1 */
    u8 local_col = col & 0x1F;  /* 0-31 */
    u8 local_row = row & 0x1F;  /* 0-31 */

    /* Offset into the 32x32 screen map */
    u16 offset = screen_row * 2 * 1024 + screen_col * 1024
               + local_row * 32 + local_col;

    /* Build the map entry */
    u16 entry = (tile_id & 0x03FF);
    if (hflip) entry |= (1 << 10);
    if (vflip) entry |= (1 << 11);
    entry |= ((u16)(palette & 0x0F) << 12);

    map[offset] = entry;
}

/* -------------------------------------------------------
 *  bg_get_map_entry — Read a tile from a BG map
 * ------------------------------------------------------- */
u16 bg_get_map_entry(int layer, u8 col, u8 row) {
    if (layer < 0 || layer > 3) return 0;

    u16 *map = bg_get_map_address(layer);
    u8 screen_col = col >> 5;
    u8 screen_row = row >> 5;
    u8 local_col = col & 0x1F;
    u8 local_row = row & 0x1F;

    u16 offset = screen_row * 2 * 1024 + screen_col * 1024
               + local_row * 32 + local_col;

    return map[offset];
}

/* -------------------------------------------------------
 *  bg_clear_map — Zero out a layer's tile map
 * ------------------------------------------------------- */
void bg_clear_map(int layer) {
    if (layer < 0 || layer > 3) return;
    u16 *map = bg_get_map_address(layer);

    /* Determine map size in 16-bit words
     * 32x32 = 1024 entries (2KB)
     * 64x64 = 4096 entries (8KB, 4 screens of 32x32) */
    u32 size;
    if (bg_configs[layer].size == BG_CNT_SCREENDIM_64x64 ||
        bg_configs[layer].size == BG_CNT_SCREENDIM_32x64 ||
        bg_configs[layer].size == BG_CNT_SCREENDIM_64x32) {
        size = 4096;
    } else {
        size = 1024;
    }

    dma_memset(map, 0x0000, size);
}