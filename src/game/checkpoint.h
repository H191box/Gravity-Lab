#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include "gba_types.h"

/* Max checkpoints per level */
#define MAX_CHECKPOINTS 10

/* Checkpoint structure */
typedef struct {
    s16 x;        /* World position (pixels) */
    s16 y;
    u16 width;    /* Gate width in pixels */
    u16 height;   /* Gate height in pixels */
} Checkpoint;

/* -------------------------------------------------------
 *  checkpoint_init — Load checkpoints for current level
 * ------------------------------------------------------- */
void checkpoint_init(u8 level_id);

/* -------------------------------------------------------
 *  checkpoint_update — Check if ship has passed through
 *  the next checkpoint in sequence.
 *
 *  sx, sy: ship world position
 *  Returns: 1 if a new checkpoint was just passed, 0 otherwise.
 * ------------------------------------------------------- */
u8 checkpoint_update(s16 sx, s16 sy);

/* -------------------------------------------------------
 *  checkpoint_get_respawn_pos — Get respawn position
 *  Returns the last passed checkpoint (or start if none).
 * ------------------------------------------------------- */
void checkpoint_get_respawn_pos(s16 *x, s16 *y, u8 *angle);

/* -------------------------------------------------------
 *  checkpoint_get_next — Get index of next checkpoint to pass
 * ------------------------------------------------------- */
u8 checkpoint_get_next(void);

/* -------------------------------------------------------
 *  checkpoint_get_total — Get total checkpoint count for level
 * ------------------------------------------------------- */
u8 checkpoint_get_total(void);

/* -------------------------------------------------------
 *  checkpoint_all_passed — Have all checkpoints been hit?
 * ------------------------------------------------------- */
bool checkpoint_all_passed(void);

/* -------------------------------------------------------
 *  checkpoint_reset — Reset to first checkpoint
 * ------------------------------------------------------- */
void checkpoint_reset(void);

/* -------------------------------------------------------
 *  checkpoint_render — Visual feedback for checkpoints
 * ------------------------------------------------------- */
void checkpoint_render(void);

#endif /* CHECKPOINT_H */