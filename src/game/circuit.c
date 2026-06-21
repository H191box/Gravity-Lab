#include "circuit.h"
#include "bg_manager.h"
#include "ship.h"
#include "math_util.h"
#include "dma.h"
#include "levels.h"

/* Camera position */
s16 camera_x = 0;
s16 camera_y = 0;

/* Current level */
const LevelData *current_level = (const LevelData *)NULL;
u8 current_level_id = 0;

/* Camera smoothing factor: 1/8 per frame */
#define CAMERA_SMOOTH 8

/* -------------------------------------------------------
 *  circuit_load — Load level tile data into BG layers
 *
 *  We render the tile map to two BG layers:
 *  - BG0: Floor, checkpoints, boost pads, finish, decorations
 *  - BG1: Walls, obstacles
 *
 *  For 64x64 maps, the BG is split into 4 32x32 screens.
 *  We iterate through each tile and place it on the correct BG.
 * ------------------------------------------------------- */
void circuit_load(u8 level_id) {
    if (level_id >= level_get_count()) return;

    current_level_id = level_id;
    current_level = level_get(level_id);

    /* Clear both BG maps */
    bg_clear_map(BG_LAYER_FLOOR);
    bg_clear_map(BG_LAYER_WALLS);

    /* Disable decoration and UI layers for gameplay */
    bg_disable(BG_LAYER_DECO);

    /* Set BG0 (floor) as highest priority, BG1 (walls) below */
    bg_set_priority(BG_LAYER_FLOOR, 0);
    bg_set_priority(BG_LAYER_WALLS, 1);
    bg_set_priority(BG_LAYER_UI, 3);

    /* Load tiles into BG maps */
    u8 w = current_level->width;
    u8 h = current_level->height;
    const u8 *map = current_level->tile_map;

    u8 row, col;
    for (row = 0; row < h; row++) {
        for (col = 0; col < w; col++) {
            u8 tile = map[row * w + col];

            /* Determine which layer this tile goes on */
            bool is_wall = (tile == TILE_WALL || tile == TILE_WALL_TOP ||
                           tile == TILE_WALL_CORNER_TL || tile == TILE_WALL_CORNER_TR ||
                           tile == TILE_WALL_CORNER_BL || tile == TILE_WALL_CORNER_BR ||
                           tile == TILE_OBSTACLE_SPAWN);

            int layer = is_wall ? BG_LAYER_WALLS : BG_LAYER_FLOOR;

            /* For wall tiles on BG1, we use a different tile offset
             * so they can have different graphics (darker/heavier look)
             */
            u16 bg_tile = (u16)tile;
            if (is_wall) {
                /* Walls use tiles 0-5 in their charbase mapping
                 * Map: WALL->0, WALL_TOP->1, CORNER_TL->2, etc. */
                switch (tile) {
                    case TILE_WALL:          bg_tile = 0; break;
                    case TILE_WALL_TOP:      bg_tile = 1; break;
                    case TILE_WALL_CORNER_TL: bg_tile = 2; break;
                    case TILE_WALL_CORNER_TR: bg_tile = 3; break;
                    case TILE_WALL_CORNER_BL: bg_tile = 4; break;
                    case TILE_WALL_CORNER_BR: bg_tile = 5; break;
                    case TILE_OBSTACLE_SPAWN: bg_tile = 6; break;
                    default:                 bg_tile = 0; break;
                }
            }

            bg_set_map_entry(layer, col, row, bg_tile, 0, FALSE, FALSE);
        }
    }

    /* Position camera at start */
    Ship *s = ship_get();
    camera_x = fixed_to_int(s->x) - SCREEN_W / 2 + 8;
    camera_y = fixed_to_int(s->y) - SCREEN_H / 2 + 8;

    /* Ensure camera is within bounds */
    u16 cw, ch;
    circuit_get_dimensions(&cw, &ch);
    if (camera_x < 0) camera_x = 0;
    if (camera_y < 0) camera_y = 0;
    if (camera_x > (s16)(cw - SCREEN_W)) camera_x = (s16)(cw - SCREEN_W);
    if (camera_y > (s16)(ch - SCREEN_H)) camera_y = (s16)(ch - SCREEN_H);
}

/* -------------------------------------------------------
 *  circuit_get_tile — Look up tile at world pixel position
 * ------------------------------------------------------- */
u8 circuit_get_tile(s16 wx, s16 wy) {
    if (!current_level) return TILE_EMPTY;

    /* Convert pixel position to tile position */
    u8 col = (u8)(wx >> 3);
    u8 row = (u8)(wy >> 3);

    if (col >= current_level->width || row >= current_level->height) {
        return TILE_EMPTY;
    }

    return current_level->tile_map[row * current_level->width + col];
}

/* -------------------------------------------------------
 *  circuit_update — Smooth camera follow + update moving elements
 * ------------------------------------------------------- */
void circuit_update(void) {
    if (!current_level) return;

    Ship *s = ship_get();

    /* Target camera position: ship at center of screen */
    s16 target_x = fixed_to_int(s->x) - SCREEN_W / 2 + 8;
    s16 target_y = fixed_to_int(s->y) - SCREEN_H / 2 + 8;

    /* Smooth interpolation toward target */
    camera_x += (target_x - camera_x) / CAMERA_SMOOTH;
    camera_y += (target_y - camera_y) / CAMERA_SMOOTH;

    /* Clamp camera within circuit bounds */
    u16 cw, ch;
    circuit_get_dimensions(&cw, &ch);

    if (cw > SCREEN_W) {
        if (camera_x < 0) camera_x = 0;
        if (camera_x > (s16)(cw - SCREEN_W)) camera_x = (s16)(cw - SCREEN_W);
    } else {
        camera_x = (s16)((cw - SCREEN_W) / 2);
    }

    if (ch > SCREEN_H) {
        if (camera_y < 0) camera_y = 0;
        if (camera_y > (s16)(ch - SCREEN_H)) camera_y = (s16)(ch - SCREEN_H);
    } else {
        camera_y = (s16)((ch - SCREEN_H) / 2);
    }
}

/* -------------------------------------------------------
 *  circuit_render — Apply camera to BG scroll registers
 *  Also applies screen shake.
 * ------------------------------------------------------- */
void circuit_render(void) {
    /* Apply screen shake offset */
    s16 shake_x = shake_get_x();
    s16 shake_y = shake_get_y();

    /* BG0 (floor) and BG1 (walls) scroll together with camera */
    bg_scroll(BG_LAYER_FLOOR, camera_x + shake_x, camera_y + shake_y);
    bg_scroll(BG_LAYER_WALLS, camera_x + shake_x, camera_y + shake_y);
}

/* -------------------------------------------------------
 *  circuit_get_start_pos — Return start position for current level
 * ------------------------------------------------------- */
void circuit_get_start_pos(s16 *x, s16 *y, u8 *angle) {
    if (!current_level) {
        if (x) *x = 128;
        if (y) *y = 80;
        if (angle) *angle = 0;
        return;
    }
    if (x) *x = current_level->start_x;
    if (y) *y = current_level->start_y;
    if (angle) *angle = current_level->start_angle;
}

/* -------------------------------------------------------
 *  circuit_is_solid — Check if a tile type blocks the ship
 * ------------------------------------------------------- */
bool circuit_is_solid(u8 tile_id) {
    switch (tile_id) {
        case TILE_WALL:
        case TILE_WALL_TOP:
        case TILE_WALL_CORNER_TL:
        case TILE_WALL_CORNER_TR:
        case TILE_WALL_CORNER_BL:
        case TILE_WALL_CORNER_BR:
            return TRUE;
        default:
            return FALSE;
    }
}

/* -------------------------------------------------------
 *  circuit_get_dimensions — Get current circuit size in pixels
 * ------------------------------------------------------- */
void circuit_get_dimensions(u16 *width, u16 *height) {
    if (!current_level) {
        if (width) *width = SCREEN_W;
        if (height) *height = SCREEN_H;
        return;
    }
    if (width) *width = (u16)current_level->width * 8;
    if (height) *height = (u16)current_level->height * 8;
}