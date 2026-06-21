#include "ship.h"
#include "input.h"
#include "sprite_manager.h"
#include "particle_system.h"
#include "fixed.h"
#include "math_util.h"
#include "circuit.h"

/* Global ship instance */
static Ship ship;

/* Camera offset (set by circuit module) */
extern s16 camera_x, camera_y;

/* -------------------------------------------------------
 *  ship_init — Set up ship at starting position
 * ------------------------------------------------------- */
void ship_init(s16 x, s16 y, u8 angle) {
    int i;

    ship.x = int_to_fixed(x);
    ship.y = int_to_fixed(y);
    ship.vx = 0;
    ship.vy = 0;
    ship.angle = angle;
    ship.state = SHIP_NORMAL;
    ship.invincible_timer = SHIP_INVINCIBLE_TIME;
    ship.tilt = 0;
    ship.boost_timer = 0;
    ship.trail_index = 0;
    ship.trail_timer = 0;

    /* Clear trail */
    for (i = 0; i < SHIP_TRAIL_LENGTH; i++) {
        ship.trail_x[i] = x;
        ship.trail_y[i] = y;
    }

    /* Allocate sprite: 16x16, starting at tile 0 */
    ship.sprite_slot = sprite_alloc(16, 16, SHIP_SPRITE_TILE,
                                     SHIP_SPRITE_PAL, SHIP_SPRITE_PRI);
}

/* -------------------------------------------------------
 *  ship_update — Main ship update per frame
 * ------------------------------------------------------- */
void ship_update(void) {
    if (ship.state == SHIP_EXPLODING) {
        return;  /* Don't process input while exploding */
    }

    if (ship.state == SHIP_RESPAWNING) {
        /* Count down invincibility */
        if (ship.invincible_timer > 0) {
            ship.invincible_timer--;
        } else {
            ship.state = SHIP_NORMAL;
        }
        /* Still allow movement while respawning */
    }

    /* Handle input */
    if (key_pressed(KEY_A)) {
        ship_thrust();
    }
    if (key_pressed(KEY_B)) {
        ship_brake();
    }
    if (key_pressed(KEY_L)) {
        ship_rotate(-1);
        ship.tilt = MAX(ship.tilt - 1, -3);
    } else if (key_pressed(KEY_R)) {
        ship_rotate(1);
        ship.tilt = MIN(ship.tilt + 1, 3);
    } else {
        /* Return tilt to center */
        if (ship.tilt > 0) ship.tilt--;
        else if (ship.tilt < 0) ship.tilt++;
    }

    /* Alternative D-pad steering */
    if (key_pressed(KEY_LEFT)) {
        ship_rotate(-1);
        ship.tilt = MAX(ship.tilt - 1, -3);
    } else if (key_pressed(KEY_RIGHT)) {
        ship_rotate(1);
        ship.tilt = MIN(ship.tilt + 1, 3);
    }
    if (key_pressed(KEY_UP)) {
        ship_thrust();
    }
    if (key_pressed(KEY_DOWN)) {
        ship_brake();
    }

    /* Apply drag (friction) */
    ship.vx = (fixed)(ship.vx * SHIP_DRAG / 256);
    ship.vy = (fixed)(ship.vy * SHIP_DRAG / 256);

    /* Clamp speed */
    fixed speed_sq = fixed_mul(ship.vx, ship.vx) + fixed_mul(ship.vy, ship.vy);
    fixed max_sq = fixed_mul(SHIP_MAX_SPEED, SHIP_MAX_SPEED);

    /* Apply boost multiplier to max speed if boosting */
    if (ship.boost_timer > 0) {
        ship.boost_timer--;
        max_sq = fixed_mul(max_sq, int_to_fixed(SHIP_BOOST_MULT));
        max_sq = fixed_div(max_sq, int_to_fixed(100));
    }

    if (speed_sq > max_sq) {
        /* Normalize velocity to max speed */
        fixed speed = fixed_sqrt(speed_sq);
        if (speed > 0) {
            ship.vx = fixed_mul(ship.vx, fixed_div(SHIP_MAX_SPEED, speed));
            ship.vy = fixed_mul(ship.vy, fixed_div(SHIP_MAX_SPEED, speed));
        }
    }

    /* Update position */
    ship.x += ship.vx;
    ship.y += ship.vy;

    /* Update trail */
    ship.trail_timer++;
    if (ship.trail_timer >= 2) {  /* Record trail every 2 frames */
        ship.trail_timer = 0;
        ship.trail_x[ship.trail_index] = fixed_to_int(ship.x);
        ship.trail_y[ship.trail_index] = fixed_to_int(ship.y);
        ship.trail_index = (ship.trail_index + 1) % SHIP_TRAIL_LENGTH;
    }

    /* Spawn exhaust particles when thrusting */
    if (key_pressed(KEY_A) || key_pressed(KEY_UP)) {
        s16 sx = fixed_to_int(ship.x) - camera_x;
        s16 sy = fixed_to_int(ship.y) - camera_y;

        /* Spawn behind the ship */
        fixed back_angle = (u8)(ship.angle + 128);
        s16 ex = sx + (s16)((sin_table[back_angle] * 8) / 127);
        s16 ey = sy - (s16)((cos_table[back_angle] * 8) / 127);

        particle_spawn(PARTICLE_EXHAUST,
                       ex + (s16)(random_next() % 5 - 2),
                       ey + (s16)(random_next() % 5 - 2),
                       (s16)((sin_table[back_angle] * 2) / 127),
                       (s16)(-(cos_table[back_angle] * 2) / 127),
                       15 + (u8)(random_next() % 10));
    }

    /* Boost glow particles */
    if (ship.boost_timer > 0) {
        s16 sx = fixed_to_int(ship.x) - camera_x;
        s16 sy = fixed_to_int(ship.y) - camera_y;
        particle_spawn(PARTICLE_BOOST_GLOW,
                       sx + (s16)(random_next() % 12 - 6),
                       sy + (s16)(random_next() % 12 - 6),
                       0, -1, 10);
    }
}

/* -------------------------------------------------------
 *  ship_render — Update sprite on screen
 * ------------------------------------------------------- */
void ship_render(void) {
    if (ship.state == SHIP_EXPLODING) {
        /* Hide ship during explosion */
        if (ship.sprite_slot >= 0) {
            sprite_set_visible(ship.sprite_slot, FALSE);
        }
        return;
    }

    if (ship.sprite_slot < 0) return;

    /* Convert world position to screen position */
    s16 screen_x = fixed_to_int(ship.x) - camera_x - 8;  /* Center 16x16 sprite */
    s16 screen_y = fixed_to_int(ship.y) - camera_y - 8;

    sprite_set_pos(ship.sprite_slot, screen_x, screen_y);

    /* Blink during invincibility */
    if (ship.invincible_timer > 0) {
        sprite_set_visible(ship.sprite_slot, (ship.invincible_timer & 3) != 0);
    } else {
        sprite_set_visible(ship.sprite_slot, TRUE);
    }

    /* Set rotation based on ship angle using affine transform */
    sprite_set_affine(ship.sprite_slot, ship.angle, FIXED_ONE, FIXED_ONE);

    /* Choose tile frame based on tilt/thrust state */
    u16 tile = SHIP_SPRITE_TILE;
    if (ship.tilt < -1) {
        tile = SHIP_SPRITE_TILE + SHIP_FRAME_LEFT * 4;
    } else if (ship.tilt > 1) {
        tile = SHIP_SPRITE_TILE + SHIP_FRAME_RIGHT * 4;
    } else if (key_pressed(KEY_A) || key_pressed(KEY_UP)) {
        tile = SHIP_SPRITE_TILE + SHIP_FRAME_THRUST * 4;
    }
    sprite_set_tile(ship.sprite_slot, tile);
}

/* -------------------------------------------------------
 *  ship_thrust — Accelerate in facing direction
 * ------------------------------------------------------- */
void ship_thrust(void) {
    /* Decompose thrust into x and y components */
    /* In GBA coords: sin = x component, -cos = y component */
    fixed thrust = int_to_fixed(SHIP_THRUST_POWER >> 4);  /* Scale down a bit */

    fixed ax = fixed_mul(thrust, (fixed)(sin_table[ship.angle] * 2));
    fixed ay = fixed_mul(-thrust, (fixed)(cos_table[ship.angle] * 2));

    ship.vx += ax;
    ship.vy += ay;
}

/* -------------------------------------------------------
 *  ship_brake — Apply reverse thrust / deceleration
 * ------------------------------------------------------- */
void ship_brake(void) {
    /* Stronger drag when braking */
    ship.vx = (fixed)(ship.vx * 230 / 256);
    ship.vy = (fixed)(ship.vy * 230 / 256);

    /* Small reverse thrust */
    fixed brake = int_to_fixed(SHIP_BRAKE_POWER >> 5);
    fixed bx = fixed_mul(brake, (fixed)(sin_table[(u8)(ship.angle + 128)] * 2));
    fixed by = fixed_mul(-brake, (fixed)(cos_table[(u8)(ship.angle + 128)] * 2));

    ship.vx += bx;
    ship.vy += by;
}

/* -------------------------------------------------------
 *  ship_rotate — Turn ship left or right
 * ------------------------------------------------------- */
void ship_rotate(s8 dir) {
    ship.angle = (u8)(ship.angle + dir * SHIP_ROTATION_SPEED);
}

/* -------------------------------------------------------
 *  ship_explode — Trigger explosion
 * ------------------------------------------------------- */
void ship_explode(void) {
    ship.state = SHIP_EXPLODING;
    ship.vx = 0;
    ship.vy = 0;

    if (ship.sprite_slot >= 0) {
        sprite_set_visible(ship.sprite_slot, FALSE);
    }
}

/* -------------------------------------------------------
 *  ship_respawn — Place ship at new position
 * ------------------------------------------------------- */
void ship_respawn(s16 x, s16 y, u8 angle) {
    ship.x = int_to_fixed(x);
    ship.y = int_to_fixed(y);
    ship.vx = 0;
    ship.vy = 0;
    ship.angle = angle;
    ship.state = SHIP_RESPAWNING;
    ship.invincible_timer = SHIP_INVINCIBLE_TIME;
    ship.tilt = 0;
    ship.boost_timer = 0;
    ship.trail_index = 0;
    ship.trail_timer = 0;

    int i;
    for (i = 0; i < SHIP_TRAIL_LENGTH; i++) {
        ship.trail_x[i] = x;
        ship.trail_y[i] = y;
    }
}

/* -------------------------------------------------------
 *  ship_get — Access ship structure
 * ------------------------------------------------------- */
Ship *ship_get(void) {
    return &ship;
}

/* -------------------------------------------------------
 *  ship_set_boost — Activate speed boost
 * ------------------------------------------------------- */
void ship_set_boost(u8 frames) {
    ship.boost_timer = frames;
}

/* -------------------------------------------------------
 *  ship_is_invincible — Check invincibility
 * ------------------------------------------------------- */
bool ship_is_invincible(void) {
    return ship.invincible_timer > 0;
}

/* -------------------------------------------------------
 *  ship_apply_boost — Speed boost from pad
 * ------------------------------------------------------- */
void ship_apply_boost(void) {
    ship_set_boost(60);  /* 1 second of boost */
}