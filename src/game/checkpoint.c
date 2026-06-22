#include "checkpoint.h"
#include "ship.h"
#include "circuit.h"
#include "particle_system.h"
#include "levels.h"
#include "system.h"
#include "math_util.h"

/* Checkpoint data */
static Checkpoint checkpoints[MAX_CHECKPOINTS];
static u8 num_checkpoints = 0;
static u8 next_checkpoint = 0;  /* Index of next checkpoint to pass */
static u8 passed_count = 0;     /* Number of checkpoints passed */

/* Start position for respawn */
static s16 start_x = 128, start_y = 80;
static u8 start_angle = 0;

/* Camera offset */
extern s16 camera_x, camera_y;

/* -------------------------------------------------------
 *  checkpoint_init — Load checkpoints from level data
 * ------------------------------------------------------- */
void checkpoint_init(u8 level_id) {
    num_checkpoints = 0;
    next_checkpoint = 0;
    passed_count = 0;

    const CheckpointDef *defs = level_get_checkpoints(level_id);
    u8 count = level_get_checkpoint_count(level_id);

    u8 i;
    for (i = 0; i < count && num_checkpoints < MAX_CHECKPOINTS; i++) {
        checkpoints[num_checkpoints].x = defs[i].x;
        checkpoints[num_checkpoints].y = defs[i].y;
        checkpoints[num_checkpoints].width = defs[i].w;
        checkpoints[num_checkpoints].height = defs[i].h;
        num_checkpoints++;
    }

    /* Get start position from level */
    s16 sx, sy;
    u8 sa;
    circuit_get_start_pos(&sx, &sy, &sa);
    start_x = sx;
    start_y = sy;
    start_angle = sa;
}

/* -------------------------------------------------------
 *  checkpoint_update — Check ship against next checkpoint
 *  Uses a simple rectangular overlap test.
 *  The ship must pass through the checkpoint gate.
 * ------------------------------------------------------- */
u8 checkpoint_update(s16 sx, s16 sy) {
    if (next_checkpoint >= num_checkpoints) return 0;

    Checkpoint *cp = &checkpoints[next_checkpoint];

    /* Check if ship center is within checkpoint rectangle */
    s16 left   = cp->x - (s16)(cp->width / 2);
    s16 right  = cp->x + (s16)(cp->width / 2);
    s16 top    = cp->y - (s16)(cp->height / 2);
    s16 bottom = cp->y + (s16)(cp->height / 2);

    if (sx >= left && sx <= right && sy >= top && sy <= bottom) {
        /* Passed this checkpoint! */
        next_checkpoint++;
        passed_count++;

        /* Visual feedback: spawn checkpoint flash particles */
        s16 screen_x = cp->x - camera_x;
        s16 screen_y = cp->y - camera_y;
        particle_spawn_burst(PARTICLE_CHECKPOINT, screen_x, screen_y,
                            8, 1, 3, 20);

        return 1;
    }

    return 0;
}

/* -------------------------------------------------------
 *  checkpoint_get_respawn_pos — Where to respawn after explosion
 * ------------------------------------------------------- */
void checkpoint_get_respawn_pos(s16 *x, s16 *y, u8 *angle) {
    if (passed_count > 0 && next_checkpoint > 0) {
        /* Respawn at last passed checkpoint */
        Checkpoint *cp = &checkpoints[next_checkpoint - 1];
        if (x) *x = cp->x;
        if (y) *y = cp->y;
        if (angle) *angle = start_angle;
    } else {
        /* Respawn at level start */
        if (x) *x = start_x;
        if (y) *y = start_y;
        if (angle) *angle = start_angle;
    }
}

/* -------------------------------------------------------
 *  checkpoint_get_next — Index of next checkpoint
 * ------------------------------------------------------- */
u8 checkpoint_get_next(void) {
    return next_checkpoint;
}

/* -------------------------------------------------------
 *  checkpoint_get_total — Total checkpoints in this level
 * ------------------------------------------------------- */
u8 checkpoint_get_total(void) {
    return num_checkpoints;
}

/* -------------------------------------------------------
 *  checkpoint_all_passed — All checkpoints cleared?
 * ------------------------------------------------------- */
bool checkpoint_all_passed(void) {
    return next_checkpoint >= num_checkpoints && num_checkpoints > 0;
}

/* -------------------------------------------------------
 *  checkpoint_reset — Reset progression
 * ------------------------------------------------------- */
void checkpoint_reset(void) {
    next_checkpoint = 0;
    passed_count = 0;
}

/* -------------------------------------------------------
 *  checkpoint_render — Draw checkpoint indicators
 *  Checkpoints are already rendered as tiles in the BG map.
 *  This adds a pulsing glow effect on the next checkpoint.
 * ------------------------------------------------------- */
void checkpoint_render(void) {
    if (next_checkpoint >= num_checkpoints) return;

    Checkpoint *cp = &checkpoints[next_checkpoint];

    /* Spawn a subtle glow at the next checkpoint every few frames */
    if (sys_get_frame_count() % 8 == 0) {
        s16 screen_x = cp->x - camera_x;
        s16 screen_y = cp->y - camera_y;

        /* Only spawn if on screen */
        if (screen_x > -32 && screen_x < SCREEN_W + 32 &&
            screen_y > -32 && screen_y < SCREEN_H + 32) {
            particle_spawn(PARTICLE_CHECKPOINT,
                          screen_x + (s16)(random_next() % (cp->width) - cp->width / 2),
                          screen_y + (s16)(random_next() % (cp->height) - cp->height / 2),
                          0, -1, 15);
        }
    }
}