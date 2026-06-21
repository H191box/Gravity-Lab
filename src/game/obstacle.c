#include "obstacle.h"
#include "sprite_manager.h"
#include "circuit.h"
#include "math_util.h"
#include "levels.h"
#include "fixed.h"

/* Obstacle pool */
static Obstacle obstacles[MAX_OBSTACLES];
static u8 num_active_obstacles = 0;

/* Camera offset (from circuit module) */
extern s16 camera_x, camera_y;

/* -------------------------------------------------------
 *  obstacle_init — Load obstacles from level data
 * ------------------------------------------------------- */
void obstacle_init(u8 level_id) {
    obstacle_clear();

    const ObstacleDef *defs = level_get_obstacles(level_id);
    u8 count = level_get_obstacle_count(level_id);

    u8 i;
    for (i = 0; i < count && num_active_obstacles < MAX_OBSTACLES; i++) {
        Obstacle *obs = &obstacles[num_active_obstacles];
        const ObstacleDef *def = &defs[i];

        obs->type = def->type;
        obs->move_pattern = def->move_pattern;
        obs->base_x = def->x;
        obs->base_y = def->y;
        obs->width = def->w;
        obs->height = def->h;
        obs->move_range = def->range;
        obs->move_speed = def->speed;
        obs->move_axis = def->axis;
        obs->start_offset = def->phase;

        /* Initial position */
        obs->x = def->x;
        obs->y = def->y;
        obs->phase = def->phase;

        /* Allocate sprite for obstacle */
        int slot = -1;
        if (obs->type == OBS_TYPE_BLADE) {
            slot = sprite_alloc(16, 16, OBJ_EXPLOSION, 0, 1);  /* Blade sprite (reuses fireball tile) */
        } else {
            slot = sprite_alloc(obs->width, obs->height, OBJ_OBSTACLE_BAR, 0, 1);  /* Bar/wall sprite */
        }
        obs->sprite_slot = (u8)(slot >= 0 ? slot : 0);
        obs->active = 1;
        num_active_obstacles++;
    }
}

/* -------------------------------------------------------
 *  obstacle_update_all — Move obstacles each frame
 * ------------------------------------------------------- */
void obstacle_update_all(void) {
    u8 i;
    for (i = 0; i < num_active_obstacles; i++) {
        Obstacle *obs = &obstacles[i];
        if (!obs->active) continue;

        obs->phase += (u8)obs->move_speed;

        switch (obs->move_pattern) {
            case OBS_MOVE_LINEAR: {
                /* Back and forth: use phase to compute position
                 * Phase 0-127: forward, 128-255: backward
                 */
                s16 offset;
                if (obs->phase < 128) {
                    offset = (s16)((obs->phase * obs->move_range) >> 7);
                } else {
                    offset = (s16)(((256 - obs->phase) * obs->move_range) >> 7);
                }

                if (obs->move_axis == 0) {
                    obs->x = obs->base_x + offset;
                } else {
                    obs->y = obs->base_y + offset;
                }
                break;
            }

            case OBS_MOVE_CIRCULAR: {
                /* Circular motion around base position */
                s16 radius = obs->move_range;
                s16 cx = (s16)((sin_table[obs->phase] * radius) / 127);
                s16 cy = (s16)((cos_table[obs->phase] * radius) / 127);
                obs->x = obs->base_x + cx;
                obs->y = obs->base_y - cy;
                break;
            }

            case OBS_MOVE_OSCILLATE: {
                /* Oscillating using sine wave */
                s16 sin_val = sin_table[obs->phase];
                s16 offset = (s16)((sin_val * obs->move_range) / 127);

                if (obs->move_axis == 0) {
                    obs->x = obs->base_x + offset;
                } else {
                    obs->y = obs->base_y + offset;
                }
                break;
            }
        }

        /* Spinning blade rotation: no position change, just the phase */
        /* (Blade visual rotation is handled in render) */
    }
}

/* -------------------------------------------------------
 *  obstacle_render_all — Update sprite positions on screen
 * ------------------------------------------------------- */
void obstacle_render_all(void) {
    u8 i;
    for (i = 0; i < num_active_obstacles; i++) {
        Obstacle *obs = &obstacles[i];
        if (!obs->active || obs->sprite_slot >= 128) continue;

        /* Convert world to screen coords */
        s16 sx = obs->x - camera_x - obs->width / 2;
        s16 sy = obs->y - camera_y - obs->height / 2;

        sprite_set_pos((int)obs->sprite_slot, sx, sy);
        sprite_set_visible((int)obs->sprite_slot, TRUE);

        /* Rotate blade sprites */
        if (obs->type == OBS_TYPE_BLADE) {
            sprite_set_affine((int)obs->sprite_slot, obs->phase, FIXED_ONE, FIXED_ONE);
        }
    }
}

/* -------------------------------------------------------
 *  obstacle_check_collision — Test ship vs all obstacles
 *  Uses circle-vs-rectangle test for accuracy.
 * ------------------------------------------------------- */
bool obstacle_check_collision(s16 sx, s16 sy, s16 sr) {
    u8 i;
    for (i = 0; i < num_active_obstacles; i++) {
        Obstacle *obs = &obstacles[i];
        if (!obs->active) continue;

        /* World position of ship */
        s16 ship_wx = sx + camera_x;
        s16 ship_wy = sy + camera_y;

        /* Build rectangle for obstacle */
        Rect obs_rect;
        obs_rect.x = obs->x - obs->width / 2;
        obs_rect.y = obs->y - obs->height / 2;
        obs_rect.w = obs->width;
        obs_rect.h = obs->height;

        if (circle_rect_overlap(ship_wx, ship_wy, sr, &obs_rect)) {
            return TRUE;
        }
    }
    return FALSE;
}

/* -------------------------------------------------------
 *  obstacle_clear — Free all obstacle sprites
 * ------------------------------------------------------- */
void obstacle_clear(void) {
    u8 i;
    for (i = 0; i < num_active_obstacles; i++) {
        if (obstacles[i].sprite_slot < 128) {
            sprite_free((int)obstacles[i].sprite_slot);
        }
    }
    num_active_obstacles = 0;
}