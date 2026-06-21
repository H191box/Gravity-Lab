#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "gba_types.h"

/* Tile types for circuit maps */
#define TILE_EMPTY           0
#define TILE_FLOOR           1
#define TILE_FLOOR_LINE_L    2   /* Floor with left edge line */
#define TILE_FLOOR_LINE_R    3   /* Floor with right edge line */
#define TILE_FLOOR_LINE_T    4   /* Floor with top edge line */
#define TILE_FLOOR_LINE_B    5   /* Floor with bottom edge line */
#define TILE_WALL            6
#define TILE_WALL_TOP        7   /* Wall with top highlight */
#define TILE_WALL_CORNER_TL  8
#define TILE_WALL_CORNER_TR  9
#define TILE_WALL_CORNER_BL  10
#define TILE_WALL_CORNER_BR  11
#define TILE_CHECKPOINT      12
#define TILE_BOOST_PAD       13
#define TILE_FINISH          14
#define TILE_OBSTACLE_SPAWN  15
#define TILE_DECO_LIGHT      16  /* Decorative floor light */
#define TILE_DECO_DARK       17  /* Dark floor accent */
#define TILE_START_POS       18  /* Starting position marker */

/* Circuit bounds */
#define CIRCUIT_MAX_WIDTH   64   /* Max tiles wide */
#define CIRCUIT_MAX_HEIGHT  64   /* Max tiles tall */
#define CIRCUIT_MAX_TILES   (CIRCUIT_MAX_WIDTH * CIRCUIT_MAX_HEIGHT)

/* Forward declaration — LevelData is defined in levels.h */
struct LevelData;

/* Camera position (world coordinates, pixel space) */
extern s16 camera_x;
extern s16 camera_y;

/* Current level data pointer */
extern const LevelData *current_level;

/* Current level index */
extern u8 current_level_id;

/* -------------------------------------------------------
 *  circuit_load — Load a level's tile map into BG0 and BG1
 *  level_id: index into levels array
 *
 *  BG0 (floor layer): Renders FLOOR, FLOOR_LINE_*, CHECKPOINT,
 *                      BOOST_PAD, FINISH, DECO_*, START_POS tiles
 *  BG1 (wall layer):  Renders WALL, WALL_*, OBSTACLE_SPAWN tiles
 * ------------------------------------------------------- */
void circuit_load(u8 level_id);

/* -------------------------------------------------------
 *  circuit_get_tile — Get tile type at world pixel position
 *  Returns the tile ID at that position.
 * ------------------------------------------------------- */
u8 circuit_get_tile(s16 wx, s16 wy);

/* -------------------------------------------------------
 *  circuit_update — Update moving elements, camera
 *  Smoothly follows the ship with interpolation.
 * ------------------------------------------------------- */
void circuit_update(void);

/* -------------------------------------------------------
 *  circuit_render — Update BG scroll positions for camera
 *  Also applies screen shake.
 * ------------------------------------------------------- */
void circuit_render(void);

/* -------------------------------------------------------
 *  circuit_get_start_pos — Get level's starting position
 * ------------------------------------------------------- */
void circuit_get_start_pos(s16 *x, s16 *y, u8 *angle);

/* -------------------------------------------------------
 *  circuit_is_solid — Check if a tile is solid (wall)
 * ------------------------------------------------------- */
bool circuit_is_solid(u8 tile_id);

/* -------------------------------------------------------
 *  circuit_get_dimensions — Get current circuit size in pixels
 * ------------------------------------------------------- */
void circuit_get_dimensions(u16 *width, u16 *height);

#endif /* CIRCUIT_H */