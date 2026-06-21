#ifndef SHIP_H
#define SHIP_H

#include "gba_types.h"
#include "fixed.h"

/* Ship states */
#define SHIP_NORMAL      0
#define SHIP_EXPLODING   1
#define SHIP_RESPAWNING  2

/* Ship constants */
#define SHIP_THRUST_POWER   180    /* Acceleration when thrusting (8.8 fixed) */
#define SHIP_BRAKE_POWER    100    /* Deceleration when braking */
#define SHIP_ROTATION_SPEED 3      /* Degrees per frame (256-degree circle) */
#define SHIP_MAX_SPEED      int_to_fixed(4)  /* Max speed: 4.0 pixels/frame */
#define SHIP_DRAG           245    /* Drag factor: 245/256 ≈ 0.957 */
#define SHIP_BOOST_MULT     140    /* Boost multiplier: 140/100 = 1.4x */
#define SHIP_COLLISION_RADIUS 6    /* Collision circle radius in pixels */
#define SHIP_INVINCIBLE_TIME 120   /* 2 seconds at 60fps */
#define SHIP_TRAIL_LENGTH   12     /* Number of trail positions */

/* Ship sprite data */
#define SHIP_SPRITE_TILE    OBJ_SHIP_CENTER  /* First tile of ship sprite (16x16 = 4 tiles) */
#define SHIP_SPRITE_PAL     0      /* OBJ palette bank */
#define SHIP_SPRITE_PRI     2      /* Priority (above BG1 walls) */

/* Ship frame indices (different visual states) */
#define SHIP_FRAME_CENTER   0  /* No tilt */
#define SHIP_FRAME_LEFT     1  /* Tilting left (4 tiles offset) */
#define SHIP_FRAME_RIGHT    2  /* Tilting right (8 tiles offset) */
#define SHIP_FRAME_THRUST   3  /* Thrusting (12 tiles offset) */

/* Ship structure */
typedef struct {
    /* Position in world coordinates (pixel space) */
    fixed x;
    fixed y;

    /* Velocity in pixels/frame (fixed-point) */
    fixed vx;
    fixed vy;

    /* Facing angle: 0=right, 64=up, 128=left, 192=down */
    u8 angle;

    /* State */
    u8 state;

    /* Invincibility timer after respawn */
    u16 invincible_timer;

    /* Sprite slot */
    int sprite_slot;

    /* Trail system: last N positions for exhaust */
    s16 trail_x[SHIP_TRAIL_LENGTH];
    s16 trail_y[SHIP_TRAIL_LENGTH];
    u8  trail_index;
    u8  trail_timer;

    /* Boost active timer */
    u8  boost_timer;

    /* Tilt visual (for animation) */
    s8  tilt;
} Ship;

/* -------------------------------------------------------
 *  ship_init — Initialize ship to starting position
 *  x, y: starting world position (integer pixels)
 *  angle: starting facing direction
 * ------------------------------------------------------- */
void ship_init(s16 x, s16 y, u8 angle);

/* -------------------------------------------------------
 *  ship_update — Update ship physics and state
 *  Handles input, physics, collision bounds checking.
 *  Call once per frame.
 * ------------------------------------------------------- */
void ship_update(void);

/* -------------------------------------------------------
 *  ship_render — Update sprite position and appearance
 *  Converts world position to screen position using camera.
 * ------------------------------------------------------- */
void ship_render(void);

/* -------------------------------------------------------
 *  ship_thrust — Apply forward thrust
 * ------------------------------------------------------- */
void ship_thrust(void);

/* -------------------------------------------------------
 *  ship_brake — Apply braking force
 * ------------------------------------------------------- */
void ship_brake(void);

/* -------------------------------------------------------
 *  ship_rotate — Rotate ship
 *  dir: -1 = left (counterclockwise), +1 = right (clockwise)
 * ------------------------------------------------------- */
void ship_rotate(s8 dir);

/* -------------------------------------------------------
 *  ship_explode — Trigger explosion state
 * ------------------------------------------------------- */
void ship_explode(void);

/* -------------------------------------------------------
 *  ship_respawn — Respawn ship at given position
 * ------------------------------------------------------- */
void ship_respawn(s16 x, s16 y, u8 angle);

/* -------------------------------------------------------
 *  ship_get — Get pointer to ship structure
 * ------------------------------------------------------- */
Ship *ship_get(void);

/* -------------------------------------------------------
 *  ship_set_boost — Activate boost for N frames
 * ------------------------------------------------------- */
void ship_set_boost(u8 frames);

/* -------------------------------------------------------
 *  ship_is_invincible — Check if ship is in invincibility
 * ------------------------------------------------------- */
bool ship_is_invincible(void);

/* -------------------------------------------------------
 *  ship_apply_boost — Called from boost pad collision
 * ------------------------------------------------------- */
void ship_apply_boost(void);

#endif /* SHIP_H */