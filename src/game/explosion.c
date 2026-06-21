#include "explosion.h"
#include "particle_system.h"
#include "sprite_manager.h"
#include "math_util.h"

/* Explosion state */
static u8  exp_active = 0;
static u8  exp_frame = 0;
static s16 exp_x = 0;
static s16 exp_y = 0;
static int exp_sprite_slot = -1;
static u16 saved_palette1 = 0;  /* Saved palette for flash restore */

/* -------------------------------------------------------
 *  explosion_start — Begin explosion sequence
 * ------------------------------------------------------- */
void explosion_start(s16 sx, s16 sy) {
    exp_active = 1;
    exp_frame = 0;
    exp_x = sx;
    exp_y = sy;

    /* Trigger screen shake */
    shake_start(EXPLOSION_SHAKE_INTENSITY, EXPLOSION_SHAKE_DURATION);

    /* Save palette and start flash */
    saved_palette1 = BG_PALETTE[1];
    BG_PALETTE[1] = 0x7FFF;  /* White flash on frame 0 */

    /* Allocate explosion sprite (16x16 fireball) */
    if (exp_sprite_slot < 0) {
        exp_sprite_slot = sprite_alloc(16, 16, OBJ_EXPLOSION, 0, 0);  /* Fireball sprite */
    }
}

/* -------------------------------------------------------
 *  explosion_update — Advance the multi-stage explosion
 * ------------------------------------------------------- */
bool explosion_update(void) {
    if (!exp_active) return FALSE;

    exp_frame++;

    if (exp_frame <= EXPLOSION_FIREBALL_DUR) {
        /* Stage 1: Expanding fireball */
        /* Fire particles expanding outward */
        if (exp_frame == 1) {
            particle_spawn_burst(PARTICLE_FIRE, exp_x, exp_y, 16, 2, 5, 20);
        }

        /* Update fireball sprite scale */
        if (exp_sprite_slot >= 0) {
            u8 scale = (u8)(256 + exp_frame * 20);  /* Growing scale */
            sprite_set_affine(exp_sprite_slot, 0, scale, scale);
            sprite_set_pos(exp_sprite_slot, exp_x - 16, exp_y - 16);
            sprite_set_visible(exp_sprite_slot, TRUE);
        }
    }
    else if (exp_frame <= EXPLOSION_FIREBALL_DUR + EXPLOSION_SPARKS_DUR) {
        /* Stage 2: Sparks flying outward */
        if (exp_frame == EXPLOSION_FIREBALL_DUR + 1) {
            /* Burst of sparks */
            particle_spawn_burst(PARTICLE_SPARK, exp_x, exp_y, 24, 3, 7, 25);

            /* Hide fireball sprite */
            if (exp_sprite_slot >= 0) {
                sprite_set_visible(exp_sprite_slot, FALSE);
            }
        }
    }
    else if (exp_frame <= EXPLOSION_TOTAL_DUR) {
        /* Stage 3: Smoke fading */
        if (exp_frame == EXPLOSION_FIREBALL_DUR + EXPLOSION_SPARKS_DUR + 1) {
            particle_spawn_burst(PARTICLE_SMOKE, exp_x, exp_y, 8, 1, 3, 30);
        }
    }
    else {
        /* Stage 4: Done — ready for respawn */
        exp_active = 0;
        exp_frame = 0;

        /* Free explosion sprite */
        if (exp_sprite_slot >= 0) {
            sprite_free(exp_sprite_slot);
            exp_sprite_slot = -1;
        }

        return TRUE;  /* Signal: explosion complete */
    }

    return FALSE;
}

/* -------------------------------------------------------
 *  explosion_render — Visual rendering pass
 *  (Most visual work is done by the particle system)
 * ------------------------------------------------------- */
void explosion_render(void) {
    /* The explosion sprite is positioned in update.
     * Additional sparks are rendered by particle_render_all().
     * This function can add extra effects if needed. */

    if (exp_active && exp_frame <= EXPLOSION_FIREBALL_DUR) {
        /* Restore palette after 2 frames of flash */
        if (exp_frame == 2) {
            BG_PALETTE[1] = saved_palette1;
        }
    }
}

/* -------------------------------------------------------
 *  explosion_is_active — Check if explosion is playing
 * ------------------------------------------------------- */
bool explosion_is_active(void) {
    return exp_active != 0;
}