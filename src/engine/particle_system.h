#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "gba_types.h"

/* Particle types */
#define PARTICLE_EXHAUST       0  /* Ship engine exhaust */
#define PARTICLE_SPARK         1  /* Explosion spark */
#define PARTICLE_FIRE          2  /* Explosion fire */
#define PARTICLE_CHECKPOINT    3  /* Checkpoint flash */
#define PARTICLE_BOOST_GLOW    4  /* Boost pad glow */
#define PARTICLE_SMOKE         5  /* Post-explosion smoke */
#define PARTICLE_MEDAL_SPARKLE 6  /* Medal earned sparkle */

/* Maximum particles in the system */
#define MAX_PARTICLES 64

/* Particle structure */
typedef struct {
    u8  type;          /* Particle type */
    u8  lifetime;      /* Remaining frames */
    u8  max_lifetime;  /* Total lifetime (for fading) */
    u8  color_id;      /* Palette color index */

    s16 x;             /* Screen position */
    s16 y;

    s16 vx;            /* Velocity (pixels/frame) */
    s16 vy;

    u8  sprite_slot;   /* Allocated sprite slot (-1 if none) */
    u8  active;        /* 1 = alive, 0 = dead */
} Particle;

/* -------------------------------------------------------
 *  particle_init — Initialize particle system
 * ------------------------------------------------------- */
void particle_init(void);

/* -------------------------------------------------------
 *  particle_spawn — Create a new particle
 *
 *  type: particle type
 *  x, y: screen position
 *  vx, vy: velocity in pixels/frame
 *  lifetime: frames to live
 *
 *  Returns particle index, or -1 if pool full.
 * ------------------------------------------------------- */
int particle_spawn(u8 type, s16 x, s16 y, s16 vx, s16 vy, u8 lifetime);

/* -------------------------------------------------------
 *  particle_spawn_burst — Create a burst of particles
 *  Used for explosions: spawns multiple sparks in all directions.
 * ------------------------------------------------------- */
void particle_spawn_burst(u8 type, s16 x, s16 y, u8 count,
                           s16 speed_min, s16 speed_max, u8 lifetime);

/* -------------------------------------------------------
 *  particle_update_all — Update all active particles
 *  Moves particles, applies gravity, decrements lifetime,
 *  frees dead particles.
 * ------------------------------------------------------- */
void particle_update_all(void);

/* -------------------------------------------------------
 *  particle_render_all — Update sprite positions for all particles
 *  Assigns/reuses sprite slots as needed.
 * ------------------------------------------------------- */
void particle_render_all(void);

/* -------------------------------------------------------
 *  particle_clear_all — Kill all particles instantly
 * ------------------------------------------------------- */
void particle_clear_all(void);

#endif /* PARTICLE_SYSTEM_H */