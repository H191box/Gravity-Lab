#include "tiles.h"
#include "gba_header.h"
#include "dma.h"

/* ============================================================
 *  Tile Graphics Generator
 *  All tiles are generated at runtime using pixel primitives.
 *  No external bitmap data needed.
 *
 *  Palette references (16-color sub-palette 0):
 *    0: Black/void
 *    1: White
 *    2: Dark gray (metal)
 *    3: Medium gray (metal highlight)
 *    4: Blue (circuit floor)
 *    5: Light blue (glow)
 *    6: Red (danger)
 *    7: Deep blue
 *    8: Green (checkpoint)
 *    9: Yellow (boost)
 *   10: Cyan (accent)
 *   11: Dark red (wall shadow)
 *   12: Orange (fire)
 *   13: Gray-blue (floor)
 *   14: Very dark blue (void)
 *   15: Light green (finish)
 * ============================================================ */

/* Helper: set a pixel in a 4bpp tile buffer
 * tile: 16 halfwords (32 bytes) for 8x8 tile
 * x, y: pixel position (0-7)
 * color: palette index (0-15)
 */
static void tile_set_pixel(u16 *tile, u8 x, u8 y, u8 color) {
    u8 shift = (x & 3) * 4;
    u8 byte_idx = (y * 4) + (x >> 2);
    tile[byte_idx] &= ~(0xF << shift);
    tile[byte_idx] |= (u16)(color & 0xF) << shift;
}

/* Helper: fill entire tile with one color */
static void tile_fill(u16 *tile, u8 color) {
    u16 val = color;
    val |= (val << 4);
    val |= (val << 8);
    val |= (val << 12);
    int i;
    for (i = 0; i < 16; i++) {
        tile[i] = val;
    }
}

/* Helper: draw a horizontal line in a tile */
static void tile_hline(u16 *tile, u8 y, u8 x1, u8 x2, u8 color) {
    u8 x;
    for (x = x1; x <= x2; x++) {
        tile_set_pixel(tile, x, y, color);
    }
}

/* Helper: draw a vertical line in a tile */
static void tile_vline(u16 *tile, u8 x, u8 y1, u8 y2, u8 color) {
    u8 y;
    for (y = y1; y <= y2; y++) {
        tile_set_pixel(tile, x, y, color);
    }
}

/* Helper: draw a box outline */
static void tile_box(u16 *tile, u8 x1, u8 y1, u8 x2, u8 y2, u8 color) {
    tile_hline(tile, y1, x1, x2, color);
    tile_hline(tile, y2, x1, x2, color);
    tile_vline(tile, x1, y1, y2, color);
    tile_vline(tile, x2, y1, y2, color);
}

/* Generate individual tile graphics */
static void gen_tile_empty(u16 *t) {
    tile_fill(t, 14);  /* Very dark blue void */
}

static void gen_tile_floor(u16 *t) {
    tile_fill(t, 4);  /* Blue floor */
    /* Subtle grid lines */
    tile_hline(t, 0, 0, 7, 3);
    tile_vline(t, 0, 0, 7, 3);
    tile_hline(t, 7, 0, 7, 2);
    tile_vline(t, 7, 0, 7, 2);
}

static void gen_tile_floor_line_l(u16 *t) {
    gen_tile_floor(t);
    tile_vline(t, 0, 0, 7, 10);  /* Bright edge on left */
}

static void gen_tile_floor_line_r(u16 *t) {
    gen_tile_floor(t);
    tile_vline(t, 7, 0, 7, 10);  /* Bright edge on right */
}

static void gen_tile_floor_line_t(u16 *t) {
    gen_tile_floor(t);
    tile_hline(t, 0, 0, 7, 10);  /* Bright edge on top */
}

static void gen_tile_floor_line_b(u16 *t) {
    gen_tile_floor(t);
    tile_hline(t, 7, 0, 7, 10);  /* Bright edge on bottom */
}

static void gen_tile_wall(u16 *t) {
    tile_fill(t, 2);  /* Dark metal */
    /* Rivets at corners */
    tile_set_pixel(t, 0, 0, 3);
    tile_set_pixel(t, 7, 0, 3);
    tile_set_pixel(t, 0, 7, 3);
    tile_set_pixel(t, 7, 7, 3);
    /* Cross lines for texture */
    tile_hline(t, 3, 2, 5, 3);
    tile_hline(t, 4, 2, 5, 11);
}

static void gen_tile_wall_top(u16 *t) {
    tile_fill(t, 2);
    tile_hline(t, 0, 0, 7, 1);  /* Highlight on top */
    tile_hline(t, 1, 0, 7, 3);
}

static void gen_tile_wall_corner_tl(u16 *t) {
    tile_fill(t, 2);
    tile_hline(t, 0, 0, 7, 1);
    tile_vline(t, 0, 0, 7, 1);
    tile_set_pixel(t, 0, 0, 10);  /* Corner accent */
}

static void gen_tile_wall_corner_tr(u16 *t) {
    tile_fill(t, 2);
    tile_hline(t, 0, 0, 7, 1);
    tile_vline(t, 7, 0, 7, 1);
    tile_set_pixel(t, 7, 0, 10);
}

static void gen_tile_wall_corner_bl(u16 *t) {
    tile_fill(t, 2);
    tile_hline(t, 7, 0, 7, 11);
    tile_vline(t, 0, 0, 7, 1);
    tile_set_pixel(t, 0, 7, 10);
}

static void gen_tile_wall_corner_br(u16 *t) {
    tile_fill(t, 2);
    tile_hline(t, 7, 0, 7, 11);
    tile_vline(t, 7, 0, 7, 1);
    tile_set_pixel(t, 7, 7, 10);
}

static void gen_tile_checkpoint(u16 *t) {
    tile_fill(t, 4);  /* Blue floor */
    /* Green gate stripes */
    u8 y;
    for (y = 0; y < 8; y++) {
        if ((y / 2) & 1) {
            tile_hline(t, y, 0, 7, 8);
        } else {
            tile_hline(t, y, 2, 5, 8);
        }
    }
}

static void gen_tile_boost_pad(u16 *t) {
    tile_fill(t, 4);  /* Blue floor */
    /* Yellow directional arrows */
    tile_hline(t, 1, 1, 6, 9);
    tile_hline(t, 2, 2, 5, 9);
    tile_set_pixel(t, 3, 3, 9);
    tile_set_pixel(t, 4, 3, 9);
    tile_set_pixel(t, 3, 4, 9);
    tile_set_pixel(t, 4, 4, 9);
    /* Glow edges */
    tile_box(t, 0, 0, 7, 7, 5);
}

static void gen_tile_finish(u16 *t) {
    /* Checkerboard pattern */
    u8 y, x;
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            if ((x + y) & 1) {
                tile_set_pixel(t, x, y, 1);  /* White */
            } else {
                tile_set_pixel(t, x, y, 2);  /* Dark */
            }
        }
    }
}

static void gen_tile_obstacle_spawn(u16 *t) {
    tile_fill(t, 4);  /* Blue floor */
    /* Red X mark */
    tile_box(t, 1, 1, 6, 6, 6);
    tile_set_pixel(t, 2, 2, 6);
    tile_set_pixel(t, 5, 2, 6);
    tile_set_pixel(t, 2, 5, 6);
    tile_set_pixel(t, 5, 5, 6);
    tile_hline(t, 3, 3, 4, 6);
    tile_hline(t, 4, 3, 4, 6);
}

static void gen_tile_deco_light(u16 *t) {
    tile_fill(t, 5);  /* Light blue glow */
    tile_box(t, 0, 0, 7, 7, 10);
    tile_hline(t, 3, 3, 4, 1);
    tile_hline(t, 4, 3, 4, 1);
}

static void gen_tile_deco_dark(u16 *t) {
    tile_fill(t, 13);  /* Gray-blue floor */
    tile_box(t, 1, 1, 6, 6, 2);
}

static void gen_tile_start(u16 *t) {
    tile_fill(t, 4);
    /* Start S marker */
    tile_box(t, 1, 1, 6, 6, 9);
    tile_hline(t, 2, 2, 5, 9);
    tile_hline(t, 3, 2, 3, 9);
    tile_hline(t, 4, 4, 5, 9);
    tile_hline(t, 5, 2, 5, 9);
}

/* -------------------------------------------------------
 *  tiles_load — Generate all tiles and copy to VRAM
 * ------------------------------------------------------- */
void tiles_load(void) {
    /* Temporary tile buffer */
    u16 tile_buf[NUM_TILES][16];
    int i;

    /* Generate each tile */
    gen_tile_empty(tile_buf[0]);
    gen_tile_floor(tile_buf[1]);
    gen_tile_floor_line_l(tile_buf[2]);
    gen_tile_floor_line_r(tile_buf[3]);
    gen_tile_floor_line_t(tile_buf[4]);
    gen_tile_floor_line_b(tile_buf[5]);
    gen_tile_wall(tile_buf[6]);
    gen_tile_wall_top(tile_buf[7]);
    gen_tile_wall_corner_tl(tile_buf[8]);
    gen_tile_wall_corner_tr(tile_buf[9]);
    gen_tile_wall_corner_bl(tile_buf[10]);
    gen_tile_wall_corner_br(tile_buf[11]);
    gen_tile_checkpoint(tile_buf[12]);
    gen_tile_boost_pad(tile_buf[13]);
    gen_tile_finish(tile_buf[14]);
    gen_tile_obstacle_spawn(tile_buf[15]);
    gen_tile_deco_light(tile_buf[16]);
    gen_tile_deco_dark(tile_buf[17]);
    gen_tile_start(tile_buf[18]);

    /* Copy to VRAM CharBase 0 (0x06000000) */
    u16 *char_base = (u16 *)BG_CHARBASE(0);

    for (i = 0; i < NUM_TILES; i++) {
        int j;
        for (j = 0; j < 16; j++) {
            char_base[i * 16 + j] = tile_buf[i][j];
        }
    }

    /* Also copy wall variants to CharBase 0 starting at tile 0
     * for the wall BG layer (BG1 shares CharBase 0).
     * The wall layer maps:
     *   wall_tile 0 = TILE_WALL (dark solid)
     *   wall_tile 1 = TILE_WALL_TOP (highlight)
     *   wall_tile 2-5 = corners
     *   wall_tile 6 = obstacle spawn
     * These are already at indices 6-11 in CharBase 0.
     * BG1 tile mapping in circuit.c re-maps them to 0-6.
     * So we also write wall tiles at indices 32-38 (second set in charbase). */

    u16 wall_tiles[7][16];
    gen_tile_wall(wall_tiles[0]);
    gen_tile_wall_top(wall_tiles[1]);
    gen_tile_wall_corner_tl(wall_tiles[2]);
    gen_tile_wall_corner_tr(wall_tiles[3]);
    gen_tile_wall_corner_bl(wall_tiles[4]);
    gen_tile_wall_corner_br(wall_tiles[5]);
    gen_tile_obstacle_spawn(wall_tiles[6]);

    for (i = 0; i < 7; i++) {
        int j;
        for (j = 0; j < 16; j++) {
            char_base[(32 + i) * 16 + j] = wall_tiles[i][j];
        }
    }
}
