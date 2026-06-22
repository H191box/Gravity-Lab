#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "gba_types.h"
#include "levels.h"

/* Tile types for circuit maps */
#define TILE_EMPTY           0
#define TILE_FLOOR           1
#define TILE_FLOOR_LINE_L    2
#define TILE_FLOOR_LINE_R    3
#define TILE_FLOOR_LINE_T    4
#define TILE_FLOOR_LINE_B    5
#define TILE_WALL            6
#define TILE_WALL_TOP        7
#define TILE_WALL_CORNER_TL  8
#define TILE_WALL_CORNER_TR  9
#define TILE_WALL_CORNER_BL  10
#define TILE_WALL_CORNER_BR  11
#define TILE_CHECKPOINT      12
#define TILE_BOOST_PAD       13
#define TILE_FINISH          14
#define TILE_OBSTACLE_SPAWN  15
#define TILE_DECO_LIGHT      16
#define TILE_DECO_DARK       17
#define TILE_START_POS       18

/* Circuit bounds */
#define CIRCUIT_MAX_WIDTH   64
#define CIRCUIT_MAX_HEIGHT  64
#define CIRCUIT_MAX_TILES   (CIRCUIT_MAX_WIDTH * CIRCUIT_MAX_HEIGHT)

/* LevelData is defined in levels.h (included before this header via gravity_lab.h) */

/* Camera position (world coordinates, pixel space) */
extern s16 camera_x;
extern s16 camera_y;

/* Current level data pointer */
extern const LevelData *current_level;

/* Current level index */
extern u8 current_level_id;

void circuit_load(u8 level_id);
u8 circuit_get_tile(s16 wx, s16 wy);
void circuit_update(void);
void circuit_render(void);
void circuit_get_start_pos(s16 *x, s16 *y, u8 *angle);
bool circuit_is_solid(u8 tile_id);
void circuit_get_dimensions(u16 *width, u16 *height);

#endif /* CIRCUIT_H */
