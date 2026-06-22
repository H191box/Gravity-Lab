#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "gba_types.h"
#include "obj_tiles.h"

/* Obstacle types */
#define OBS_TYPE_HBAR         0   /* Horizontal moving bar */
#define OBS_TYPE_VBAR         1   /* Vertical moving bar */
#define OBS_TYPE_BLADE        2   /* Spinning blade */
#define OBS_TYPE_MOVING_WALL  3   /* Moving wall segment */

/* Obstacle movement patterns */
#define OBS_MOVE_LINEAR       0   /* Back and forth in a line */
#define OBS_MOVE_CIRCULAR     1   /* Circular motion */
#define OBS_MOVE_OSCILLATE    2   /* Oscillating (sine-like) */

/* Max obstacles per level */
#define MAX_OBSTACLES 16

/* Obstacle definition (from level data) */
typedef struct {
    u8  type;           /* OBS_TYPE_* */
    u8  move_pattern;   /* OBS_MOVE_* */

    /* Base position in pixels */
    s16 base_x;
    s16 base_y;

    /* Size in pixels */
    u8  width;
    u8  height;

    /* Movement parameters */
    s16 move_range;     /* Range of movement in pixels */
    s16 move_speed;     /* Speed: pixels per frame */
    u8  move_axis;      /* 0 = horizontal, 1 = vertical */
    u8  start_offset;   /* Phase offset (0-255) */

    /* Current state (updated at runtime) */
    s16 x;
    s16 y;
    u8  phase;          /* Current phase in movement cycle */
    u8  sprite_slot;    /* Allocated sprite slot */
    u8  active;         /* Whether this obstacle is currently in the level */
} Obstacle;

/* -------------------------------------------------------
 *  obstacle_init — Load obstacles for the current level
 *  level_id: current level index
 * ------------------------------------------------------- */
void obstacle_init(u8 level_id);

/* -------------------------------------------------------
 *  obstacle_update_all — Update all active obstacles
 *  Moves obstacles according to their patterns.
 * ------------------------------------------------------- */
void obstacle_update_all(void);

/* -------------------------------------------------------
 *  obstacle_render_all — Update sprite positions
 * ------------------------------------------------------- */
void obstacle_render_all(void);

/* -------------------------------------------------------
 *  obstacle_check_collision — Check if ship collides with any obstacle
 *  sx, sy: ship center position in screen pixels
 *  sr: ship collision radius
 *  Returns TRUE if collision detected.
 * ------------------------------------------------------- */
bool obstacle_check_collision(s16 sx, s16 sy, s16 sr);

/* -------------------------------------------------------
 *  obstacle_clear — Clear all obstacles
 * ------------------------------------------------------- */
void obstacle_clear(void);

#endif /* OBSTACLE_H */