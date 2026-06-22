#ifndef MEDAL_SYSTEM_H
#define MEDAL_SYSTEM_H

#include "gba_types.h"
#include "obj_tiles.h"

/* Medal types */
#define MEDAL_NONE   0
#define MEDAL_BRONZE 1
#define MEDAL_SILVER 2
#define MEDAL_GOLD   3

/* -------------------------------------------------------
 *  medal_calculate — Determine medal for a completion time
 *  time_cs: time in centiseconds
 *  level_id: which level
 *  Returns: MEDAL_GOLD, MEDAL_SILVER, MEDAL_BRONZE, or MEDAL_NONE
 * ------------------------------------------------------- */
u8 medal_calculate(u32 time_cs, u8 level_id);

/* -------------------------------------------------------
 *  medal_render — Draw a medal sprite at screen position
 *  Uses a 16x16 sprite.
 *  type: MEDAL_GOLD/SILVER/BRONZE
 *  screen_x, screen_y: center position on screen
 * ------------------------------------------------------- */
void medal_render(u8 type, s16 screen_x, s16 screen_y);

/* -------------------------------------------------------
 *  medal_get_name — Get medal name string
 *  Returns: "GOLD", "SILVER", "BRONZE", or "NONE"
 * ------------------------------------------------------- */
const char *medal_get_name(u8 type);

/* -------------------------------------------------------
 *  medal_get_color — Get text color for medal type
 *  Returns palette index for text rendering
 * ------------------------------------------------------- */
u8 medal_get_color(u8 type);

#endif /* MEDAL_SYSTEM_H */