#include "sprites.h"
#include "dma.h"

/* ============================================================
 *  Sprite Graphics Generator
 *  All sprites are generated at runtime using pixel primitives.
 *  4bpp format: 8x8 tile = 32 bytes = 16 halfwords
 *  16x16 sprite = 4 tiles = 128 bytes = 64 halfwords
 *
 *  OBJ Palette 0 colors used:
 *    0: Transparent
 *    1: White (ship body)
 *    2: Cyan (ship outline/glow)
 *    3: Red (thrust)
 *    4: Yellow (spark/fire)
 *    5: Orange (flame)
 *    6: Silver (metal parts)
 *    7: Dark orange (shadow)
 *    8: Deep blue (cockpit)
 *    9: Green (indicator)
 *   10: Light gray (smoke)
 *   11: Dark cyan
 *   12: Gold
 * ============================================================ */

/* OBJ VRAM in 1D sprite mapping mode:
 * Tile index N maps to VRAM address 0x06000000 + N * 32 bytes.
 * BG CharBase 0 uses tiles 0-511 (0x00000-0x0FFFF = 64KB).
 * We place OBJ tiles starting at tile 512 to avoid overlap.
 * VRAM address = 0x06000000 + 512 * 32 = 0x06004000.
 */
#define OBJ_VRAM_TILE_START  512
#define OBJ_VRAM_BASE        ((u16 *)(0x06000000 + OBJ_VRAM_TILE_START * 32))

/* Helper: set pixel in 4bpp sprite tile data */
static void spr_pixel(u16 *data, u8 x, u8 y, u8 color, u8 tile_w) {
    /* For a multi-tile sprite, tile_w is in tiles (e.g., 2 for 16x16) */
    u8 tile_col = x >> 3;
    u8 tile_row = y >> 3;
    u8 local_x = x & 7;
    u8 local_y = y & 7;

    u16 offset = (tile_row * tile_w + tile_col) * 32 + local_y * 4 + (local_x >> 1);
    u8 shift = (local_x & 1) * 4;
    data[offset] &= ~(0xF << shift);
    data[offset] |= (u16)(color & 0xF) << shift;
}

/* Helper: fill a rectangular area with a color */
static void spr_rect(u16 *data, u8 x, u8 y, u8 w, u8 h, u8 color, u8 tile_w) {
    u8 py, px;
    for (py = 0; py < h; py++) {
        for (px = 0; px < w; px++) {
            spr_pixel(data, x + px, y + py, color, tile_w);
        }
    }
}

/* Helper: draw a circle */
static void spr_circle(u16 *data, u8 cx, u8 cy, u8 r, u8 color, u8 tile_w) {
    s16 dx, dy;
    for (dy = -(s16)r; dy <= (s16)r; dy++) {
        for (dx = -(s16)r; dx <= (s16)r; dx++) {
            if (dx * dx + dy * dy <= r * r) {
                u8 px = (u8)(cx + dx);
                u8 py = (u8)(cy + dy);
                spr_pixel(data, px, py, color, tile_w);
            }
        }
    }
}

/* Helper: draw a line */
static void spr_line(u16 *data, u8 x1, u8 y1, u8 x2, u8 y2, u8 color, u8 tile_w) {
    s16 dx = (s16)x2 - (s16)x1;
    s16 dy = (s16)y2 - (s16)y1;
    s16 steps = ABS(dx) > ABS(dy) ? ABS(dx) : ABS(dy);
    if (steps == 0) return;
    s16 i;
    for (i = 0; i <= steps; i++) {
        u8 px = (u8)((s16)x1 + dx * i / steps);
        u8 py = (u8)((s16)y1 + dy * i / steps);
        spr_pixel(data, px, py, color, tile_w);
    }
}

/* 16x16 sprite generation — each uses 4 tiles (64 halfwords) */
static void gen_ship_normal(u16 *data) {
    /* Ship pointing right: sleek triangle shape with detail */
    u16 *d = data;

    /* Clear all to transparent */
    int i;
    for (i = 0; i < 64; i++) d[i] = 0;

    /* Main body: triangle pointing right (0) */
    /* Outline */
    spr_line(d, 15, 8, 2, 2, 2, 2);   /* Top edge */
    spr_line(d, 15, 8, 2, 14, 2, 2);  /* Bottom edge */
    spr_line(d, 2, 2, 2, 14, 2, 2);    /* Back edge */

    /* Fill body */
    spr_rect(d, 4, 4, 10, 8, 1, 2);   /* White body center */
    spr_rect(d, 3, 5, 2, 6, 1, 2);    /* Body narrow part */

    /* Cockpit */
    spr_rect(d, 10, 6, 3, 4, 8, 2);   /* Blue cockpit */
    spr_pixel(d, 10, 7, 2, 2);          /* Cockpit edge */
    spr_pixel(d, 10, 8, 2, 2);

    /* Engine glow at back */
    spr_rect(d, 1, 6, 2, 4, 6, 2);    /* Silver engine */

    /* Wing accents */
    spr_line(d, 5, 3, 8, 4, 2, 2);     /* Top wing line */
    spr_line(d, 5, 13, 8, 12, 2, 2);   /* Bottom wing line */
}

static void gen_ship_thrust(u16 *data) {
    /* Same as normal but with thrust flame at back */
    gen_ship_normal(data);
    /* Add flame */
    spr_rect(data, 0, 6, 2, 2, 3, 2);   /* Red flame core */
    spr_pixel(data, 0, 5, 5, 2);         /* Orange tip */
    spr_pixel(data, 0, 9, 5, 2);
    spr_pixel(data, 0, 7, 4, 2);         /* Yellow glow */
    spr_pixel(data, 0, 8, 4, 2);
}

static void gen_ship_tilt_left(u16 *data) {
    gen_ship_normal(data);
    /* Shift visual slightly to indicate left tilt */
    /* Add tilt indicator line on top */
    spr_line(data, 6, 3, 10, 2, 9, 2);  /* Green indicator */
}

static void gen_ship_tilt_right(u16 *data) {
    gen_ship_normal(data);
    /* Add tilt indicator on bottom */
    spr_line(data, 6, 13, 10, 14, 9, 2);
}

static void gen_explosion_fireball(u16 *data) {
    int i;
    for (i = 0; i < 64; i++) data[i] = 0;

    /* Expanding fireball — bright center, orange/red edges */
    spr_circle(data, 8, 8, 7, 5, 2);     /* Orange outer */
    spr_circle(data, 8, 8, 5, 4, 2);     /* Yellow middle */
    spr_circle(data, 8, 8, 3, 1, 2);     /* White core */

    /* Random fiery edges */
    spr_pixel(data, 0, 8, 3, 2);
    spr_pixel(data, 1, 4, 5, 2);
    spr_pixel(data, 1, 12, 5, 2);
    spr_pixel(data, 15, 7, 3, 2);
    spr_pixel(data, 14, 3, 5, 2);
    spr_pixel(data, 14, 13, 5, 2);
    spr_pixel(data, 8, 0, 4, 2);
    spr_pixel(data, 8, 15, 4, 2);
}

static void gen_obstacle_bar(u16 *data) {
    int i;
    for (i = 0; i < 64; i++) data[i] = 0;

    /* Red hazard bar */
    spr_rect(data, 2, 3, 12, 10, 6, 2);  /* Dark red body */
    spr_rect(data, 3, 4, 10, 8, 3, 2);   /* Bright red core */
    /* Hazard stripes */
    u8 j;
    for (j = 0; j < 10; j += 2) {
        spr_line(data, 3 + j, 4, 4 + j, 11, 7, 2);
    }
}

static void gen_medal(u16 *data, u8 pal_offset) {
    int i;
    for (i = 0; i < 64; i++) data[i] = 0;

    /* Medal shape: circle with inner detail */
    spr_circle(data, 8, 8, 7, pal_offset + 1, 2);  /* Medal body */
    spr_circle(data, 8, 8, 5, pal_offset + 2, 2);   /* Inner ring */
    spr_circle(data, 8, 8, 2, pal_offset + 1, 2);   /* Center dot */

    /* Ribbon at top */
    spr_rect(data, 5, 1, 6, 2, pal_offset + 3, 2);
    spr_rect(data, 6, 0, 4, 2, pal_offset + 1, 2);

    /* Shine highlight */
    spr_pixel(data, 5, 5, 0, 2);  /* Transparent for highlight */
    /* Actually let's use a bright pixel for shine */
    spr_pixel(data, 5, 5, 15, 2);  /* Light blue shine */
    spr_pixel(data, 5, 6, 15, 2);
}

static void gen_particle_spark(u16 *data) {
    /* 8x8 spark particle — simple bright dot */
    data[0] = 0;
    int i;
    for (i = 1; i < 16; i++) data[i] = 0;
    spr_circle(data, 4, 4, 3, 4, 1);   /* Yellow center */
    spr_pixel(data, 4, 4, 1, 1);          /* White hot center */
}

static void gen_particle_glow(u16 *data) {
    int i;
    for (i = 0; i < 16; i++) data[i] = 0;
    spr_circle(data, 4, 4, 3, 15, 1);  /* Cyan glow */
    spr_circle(data, 4, 4, 1, 1, 1);    /* White center */
}

static void gen_particle_smoke(u16 *data) {
    int i;
    for (i = 0; i < 16; i++) data[i] = 0;
    spr_circle(data, 4, 4, 3, 10, 1);  /* Gray smoke */
    spr_circle(data, 4, 4, 2, 6, 1);   /* Darker center */
}

static void gen_particle_checkpoint(u16 *data) {
    int i;
    for (i = 0; i < 16; i++) data[i] = 0;
    /* Green diamond/flag marker */
    spr_pixel(data, 4, 0, 9, 1);
    spr_line(data, 3, 1, 5, 1, 9, 1);
    spr_line(data, 2, 2, 6, 2, 9, 1);
    spr_line(data, 3, 3, 5, 3, 9, 1);
    spr_pixel(data, 4, 4, 9, 1);
}

/* -------------------------------------------------------
 *  sprites_load — Generate all sprites and write to OBJ VRAM
 * ------------------------------------------------------- */
void sprites_load(void) {
    /* 16x16 sprites: 64 halfwords each */
    u16 buf[64];

    /* Ship frame 0: Normal (tiles 0-3) */
    gen_ship_normal(buf);
    int i;
    for (i = 0; i < 64; i++) OBJ_VRAM_BASE[0 * 64 + i] = buf[i];

    /* Ship frame 1: Tilt left (tiles 4-7) */
    gen_ship_tilt_left(buf);
    for (i = 0; i < 64; i++) OBJ_VRAM_BASE[1 * 64 + i] = buf[i];

    /* Ship frame 2: Tilt right (tiles 8-11) */
    gen_ship_tilt_right(buf);
    for (i = 0; i < 64; i++) OBJ_VRAM_BASE[2 * 64 + i] = buf[i];

    /* Ship frame 3: Thrust (tiles 12-15) */
    gen_ship_thrust(buf);
    for (i = 0; i < 64; i++) OBJ_VRAM_BASE[3 * 64 + i] = buf[i];

    /* Explosion fireball (tiles 16-19) */
    gen_explosion_fireball(buf);
    for (i = 0; i < 64; i++) OBJ_VRAM_BASE[4 * 64 + i] = buf[i];

    /* Obstacle bar (tiles 20-23) */
    gen_obstacle_bar(buf);
    for (i = 0; i < 64; i++) OBJ_VRAM_BASE[5 * 64 + i] = buf[i];

    /* Gold medal (tiles 24-27) — uses OBJ palette bank 1 */
    gen_medal(buf, 16);  /* Offset into OBJ palette */
    /* But medals use sub-palette 1 for gold, etc.
     * We'll store them with color indices 1-3 relative to their sub-palette.
     * Since the medal sprites are allocated with specific palette banks,
     * we generate them using colors 1-3 (which map to the sub-palette's entries). */
    gen_medal(buf, 0);  /* Colors 1-3 will be palette-relative */
    for (i = 0; i < 64; i++) OBJ_VRAM_BASE[6 * 64 + i] = buf[i];

    /* Silver medal (tiles 28-31) */
    gen_medal(buf, 0);
    for (i = 0; i < 64; i++) OBJ_VRAM_BASE[7 * 64 + i] = buf[i];

    /* Bronze medal (tiles 32-35) */
    gen_medal(buf, 0);
    for (i = 0; i < 64; i++) OBJ_VRAM_BASE[8 * 64 + i] = buf[i];

    /* 8x8 particles: 16 halfwords each */
    /* Particle spark (tile 36) */
    u16 small_buf[16];
    gen_particle_spark(small_buf);
    for (i = 0; i < 16; i++) OBJ_VRAM_BASE[36 * 16 + i] = small_buf[i];

    /* Particle glow (tile 37) */
    gen_particle_glow(small_buf);
    for (i = 0; i < 16; i++) OBJ_VRAM_BASE[37 * 16 + i] = small_buf[i];

    /* Particle smoke (tile 38) */
    gen_particle_smoke(small_buf);
    for (i = 0; i < 16; i++) OBJ_VRAM_BASE[38 * 16 + i] = small_buf[i];

    /* Checkpoint marker (tile 39) */
    gen_particle_checkpoint(small_buf);
    for (i = 0; i < 16; i++) OBJ_VRAM_BASE[39 * 16 + i] = small_buf[i];
}
