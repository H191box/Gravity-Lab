#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "gba_types.h"
#include "obj_tiles.h"

/* Explosion stages */
#define EXPLOSION_STAGE_FIREBALL  0   /* Frames 0-5 */
#define EXPLOSION_STAGE_SPARKS    1   /* Frames 6-15 */
#define EXPLOSION_STAGE_SMOKE     2   /* Frames 16-20 */
#define EXPLOSION_STAGE_DONE      3   /* Frame 21: ready to respawn */

/* Explosion timing */
#define EXPLOSION_FIREBALL_DUR   6
#define EXPLOSION_SPARKS_DUR     10
#define EXPLOSION_SMOKE_DUR      5
#define EXPLOSION_TOTAL_DUR      22

/* Screen shake params */
#define EXPLOSION_SHAKE_INTENSITY 6
#define EXPLOSION_SHAKE_DURATION  12

/* -------------------------------------------------------
 *  explosion_start — Trigger explosion at screen position
 *  sx, sy: screen coordinates of explosion center
 * ------------------------------------------------------- */
void explosion_start(s16 sx, s16 sy);

/* -------------------------------------------------------
 *  explosion_update — Advance explosion animation
 *  Returns: TRUE when explosion is complete (ready for respawn)
 * ------------------------------------------------------- */
bool explosion_update(void);

/* -------------------------------------------------------
 *  explosion_render — Render explosion visuals
 *  Uses particle system + sprite.
 * ------------------------------------------------------- */
void explosion_render(void);

/* -------------------------------------------------------
 *  explosion_is_active — Is explosion currently playing?
 * ------------------------------------------------------- */
bool explosion_is_active(void);

#endif /* EXPLOSION_H */