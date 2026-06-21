#ifndef TIMER_DISPLAY_H
#define TIMER_DISPLAY_H

#include "gba_types.h"

/* Timer state */
typedef struct {
    u32 centiseconds;      /* Elapsed time in 1/100th seconds */
    bool running;          /* Is the timer counting? */
    bool started;          /* Has the timer ever been started? */
} GameTimer;

/* -------------------------------------------------------
 *  timer_display_init — Initialize the game timer
 *  Sets up hardware timers.
 * ------------------------------------------------------- */
void timer_display_init(void);

/* -------------------------------------------------------
 *  gt_start — Begin / resume counting
 * ------------------------------------------------------- */
void gt_start(void);

/* -------------------------------------------------------
 *  gt_stop — Pause the timer
 * ------------------------------------------------------- */
void gt_stop(void);

/* -------------------------------------------------------
 *  gt_reset — Reset to zero and stop
 * ------------------------------------------------------- */
void gt_reset(void);

/* -------------------------------------------------------
 *  gt_get_time — Get current elapsed time in centiseconds
 * ------------------------------------------------------- */
u32 gt_get_time(void);

/* -------------------------------------------------------
 *  gt_render — Draw the timer to the HUD
 *  x, y: tile position on text layer
 *  color: text color index
 * ------------------------------------------------------- */
void gt_render(u8 x, u8 y, u8 color);

/* -------------------------------------------------------
 *  gt_save_best — Save best time to SRAM for a level
 *  Returns TRUE if new best time.
 * ------------------------------------------------------- */
bool gt_save_best(u8 level_id, u32 time_cs);

/* -------------------------------------------------------
 *  gt_load_best — Load best time from SRAM
 *  Returns 0 if no time saved.
 * ------------------------------------------------------- */
u32 gt_load_best(u8 level_id);

#endif /* TIMER_DISPLAY_H */