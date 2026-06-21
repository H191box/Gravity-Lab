#include "particle_system.h"
#include "sprite_manager.h"
#include "math_util.h"

/* Particle pool */
static Particle particles[MAX_PARTICLES];

/* Tile indices for particle sprites (set during init) */
static u16 particle_tiles[8] = {0};  /* One tile per particle type */

/* -------------------------------------------------------
 *  particle_init — Clear all particles
 * ------------------------------------------------------- */
void particle_init(void) {
    int i;
    for (i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = 0;
        particles[i].sprite_slot = (u8)-1;
    }

    /* Define tile indices for each particle type
     * These correspond to sprite tiles loaded in VRAM.
     * Layout in OBJ tile data:
     *   Tile 0-1: Ship (16x16 = 4 tiles, but ship uses affine so 4 tiles)
     *   Tile 4-7: Explosion frame (16x16 = 4 tiles)
     *   Tile 8: Spark (8x8)
     *   Tile 9: Glow (8x8)
     *   Tile 10: Smoke (8x8)
     *   Tile 11: Checkpoint flag (8x8)
     *   Tile 12-15: Medal sprites (16x16 each = 4 tiles)
     */
    particle_tiles[PARTICLE_EXHAUST]       = 8;   /* 8x8 spark, color 15 (blue) */
    particle_tiles[PARTICLE_SPARK]         = 8;   /* 8x8 spark, color 4 (yellow) */
    particle_tiles[PARTICLE_FIRE]          = 9;   /* 8x8 glow, color 5 (orange) */
    particle_tiles[PARTICLE_CHECKPOINT]    = 11;  /* 8x8 flag */
    particle_tiles[PARTICLE_BOOST_GLOW]    = 9;   /* 8x8 glow, color 15 (cyan) */
    particle_tiles[PARTICLE_SMOKE]         = 10;  /* 8x8 smoke */
    particle_tiles[PARTICLE_MEDAL_SPARKLE] = 8;   /* 8x8 spark, color 12 (gold) */
}

/* -------------------------------------------------------
 *  particle_spawn — Create a single particle
 * ------------------------------------------------------- */
int particle_spawn(u8 type, s16 x, s16 y, s16 vx, s16 vy, u8 lifetime) {
    int i;
    for (i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            Particle *p = &particles[i];
            p->active = 1;
            p->type = type;
            p->x = x;
            p->y = y;
            p->vx = vx;
            p->vy = vy;
            p->lifetime = lifetime;
            p->max_lifetime = lifetime;
            p->sprite_slot = (u8)-1;

            /* Set color based on type */
            switch (type) {
                case PARTICLE_EXHAUST:
                    p->color_id = 15;  /* Cyan/blue */
                    break;
                case PARTICLE_SPARK:
                    p->color_id = 4;   /* Yellow */
                    break;
                case PARTICLE_FIRE:
                    p->color_id = 5;   /* Orange */
                    break;
                case PARTICLE_CHECKPOINT:
                    p->color_id = 9;   /* Green */
                    break;
                case PARTICLE_BOOST_GLOW:
                    p->color_id = 15;  /* Cyan */
                    break;
                case PARTICLE_SMOKE:
                    p->color_id = 10;  /* Gray */
                    break;
                case PARTICLE_MEDAL_SPARKLE:
                    p->color_id = 12;  /* Gold */
                    break;
                default:
                    p->color_id = 1;
                    break;
            }

            return i;
        }
    }
    return -1;
}

/* -------------------------------------------------------
 *  particle_spawn_burst — Create particles in a radial pattern
 * ------------------------------------------------------- */
void particle_spawn_burst(u8 type, s16 x, s16 y, u8 count,
                           s16 speed_min, s16 speed_max, u8 lifetime) {
    u8 i;
    for (i = 0; i < count; i++) {
        /* Random angle in 256-degree circle */
        u8 angle = (u8)(random_next() & 0xFF);
        s16 speed = (s16)(speed_min + (random_next() % (u32)(speed_max - speed_min + 1)));

        /* Decompose into vx, vy */
        s16 vx = (s16)((sin_table[angle] * speed * 2) / 256);
        s16 vy = (s16)((-cos_table[angle] * speed * 2) / 256);  /* -cos because Y is inverted */

        particle_spawn(type, x, y, vx, vy, lifetime);
    }
}

/* -------------------------------------------------------
 *  particle_update_all — Physics update for all particles
 * ------------------------------------------------------- */
void particle_update_all(void) {
    int i;
    for (i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &particles[i];
        if (!p->active) continue;

        /* Move */
        p->x += p->vx;
        p->y += p->vy;

        /* Type-specific physics */
        switch (p->type) {
            case PARTICLE_EXHAUST:
                /* Slight deceleration and rise */
                p->vx = (s16)(p->vx * 14 / 16);
                p->vy = (s16)(p->vy * 14 / 16) - 1;
                break;

            case PARTICLE_SPARK:
                /* Gravity pulls down */
                p->vy += 2;
                p->vx = (s16)(p->vx * 15 / 16);
                break;

            case PARTICLE_FIRE:
                /* Expand outward, slight deceleration */
                p->vx = (s16)(p->vx * 12 / 16);
                p->vy = (s16)(p->vy * 12 / 16) - 1;
                break;

            case PARTICLE_BOOST_GLOW:
                /* Float upward */
                p->vy -= 1;
                p->vx = (s16)(p->vx * 15 / 16);
                break;

            case PARTICLE_SMOKE:
                /* Slow drift upward with random horizontal wobble */
                p->vy -= 1;
                p->vx += (s16)(random_next() % 3) - 1;
                p->vx = (s16)(p->vx * 14 / 16);
                break;

            case PARTICLE_CHECKPOINT:
                /* Rise quickly */
                p->vy -= 3;
                p->vx = (s16)(p->vx * 12 / 16);
                break;

            case PARTICLE_MEDAL_SPARKLE:
                /* Sparkle: small random movement */
                p->vx += (s16)(random_next() % 3) - 1;
                p->vy += (s16)(random_next() % 3) - 1;
                break;
        }

        /* Decrement lifetime */
        if (p->lifetime > 0) {
            p->lifetime--;
        }

        /* Kill if off-screen or expired */
        if (p->lifetime == 0 ||
            p->x < -32 || p->x > SCREEN_W + 32 ||
            p->y < -32 || p->y > SCREEN_H + 32) {
            /* Free sprite slot if allocated */
            if (p->sprite_slot != (u8)-1) {
                sprite_free(p->sprite_slot);
                p->sprite_slot = (u8)-1;
            }
            p->active = 0;
        }
    }
}

/* -------------------------------------------------------
 *  particle_render_all — Assign sprites to active particles
 *  Particles use 8x8 sprites from shared tile data.
 * ------------------------------------------------------- */
void particle_render_all(void) {
    /* First, release all existing particle sprites so we can re-allocate */
    /* (We do this every frame for simplicity; particles are transient) */
    int i;

    /* Free old sprite slots */
    for (i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active && particles[i].sprite_slot != (u8)-1) {
            sprite_free(particles[i].sprite_slot);
            particles[i].sprite_slot = (u8)-1;
        }
    }

    /* Allocate sprites for active particles */
    for (i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &particles[i];
        if (!p->active) continue;

        /* Allocate an 8x8 sprite */
        int slot = sprite_alloc(8, 8, particle_tiles[p->type], 0, 1);
        if (slot < 0) continue;  /* Out of sprite slots */

        p->sprite_slot = (u8)slot;

        /* Set position (8x8 sprites are offset by -4 from center) */
        sprite_set_pos(slot, p->x - 4, p->y - 4);

        /* Fade effect: make sprite invisible near end of life */
        /* We can't easily fade in hardware, but we can hide near end */
        if (p->lifetime < p->max_lifetime / 4) {
            /* Blink near death */
            if ((p->lifetime & 1) == 0) {
                sprite_set_visible(slot, FALSE);
            }
        }
    }
}

/* -------------------------------------------------------
 *  particle_clear_all — Kill every particle, free sprites
 * ------------------------------------------------------- */
void particle_clear_all(void) {
    int i;
    for (i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].sprite_slot != (u8)-1) {
            sprite_free(particles[i].sprite_slot);
        }
        particles[i].active = 0;
        particles[i].sprite_slot = (u8)-1;
    }
}