#ifndef LEVELS_H
#define LEVELS_H

#include "gba_types.h"

/* Obstacle definition (used in level data) */
typedef struct {
    u8  type;       /* OBS_TYPE_* */
    u8  axis;       /* 0=horizontal, 1=vertical */
    s16 x;          /* Base position (pixels) */
    s16 y;
    u8  w;          /* Width/height */
    u8  h;
    s16 range;      /* Movement range */
    s16 speed;      /* Movement speed */
    u8  phase;      /* Phase offset */
} ObstacleDef;

/* Checkpoint definition */
typedef struct {
    s16 x;          /* Position (pixels) */
    s16 y;
    u16 w;          /* Gate width */
    u16 h;          /* Gate height */
} CheckpointDef;

/* Level data structure */
typedef struct {
    const u8 *tile_map;
    u8  width;
    u8  height;
    s16 start_x;
    s16 start_y;
    u8  start_angle;
    const char *name;
    const ObstacleDef *obstacles;
    u8  num_obstacles;
    const CheckpointDef *checkpoints;
    u8  num_checkpoints;
} LevelData;

/* -------------------------------------------------------
 *  level_get — Get level data by index
 * ------------------------------------------------------- */
const LevelData *level_get(u8 id);

/* -------------------------------------------------------
 *  level_get_count — Total number of levels
 * ------------------------------------------------------- */
u8 level_get_count(void);

/* -------------------------------------------------------
 *  level_get_name — Get level name string
 * ------------------------------------------------------- */
const char *level_get_name(u8 id);

/* -------------------------------------------------------
 *  level_get_obstacles — Get obstacle array for level
 * ------------------------------------------------------- */
const ObstacleDef *level_get_obstacles(u8 id);

/* -------------------------------------------------------
 *  level_get_obstacle_count — Number of obstacles
 * ------------------------------------------------------- */
u8 level_get_obstacle_count(u8 id);

/* -------------------------------------------------------
 *  level_get_checkpoints — Get checkpoint array for level
 * ------------------------------------------------------- */
const CheckpointDef *level_get_checkpoints(u8 id);

/* -------------------------------------------------------
 *  level_get_checkpoint_count — Number of checkpoints
 * ------------------------------------------------------- */
u8 level_get_checkpoint_count(u8 id);

/* -------------------------------------------------------
 *  level_get_start_pos — Get start position for a level
 *  (used by game_state before circuit_load)
 * ------------------------------------------------------- */
void level_get_start_pos(u8 id, s16 *x, s16 *y, u8 *angle);

/* Level tile maps (defined in levels.c) */
extern const u8 level1_map[];
extern const u8 level2_map[];
extern const u8 level3_map[];
extern const u8 level4_map[];
extern const u8 level5_map[];

/* Obstacle arrays */
extern const ObstacleDef level1_obstacles[];
extern const ObstacleDef level2_obstacles[];
extern const ObstacleDef level3_obstacles[];
extern const ObstacleDef level4_obstacles[];
extern const ObstacleDef level5_obstacles[];

/* Checkpoint arrays */
extern const CheckpointDef level1_checkpoints[];
extern const CheckpointDef level2_checkpoints[];
extern const CheckpointDef level3_checkpoints[];
extern const CheckpointDef level4_checkpoints[];
extern const CheckpointDef level5_checkpoints[];

#endif /* LEVELS_H */