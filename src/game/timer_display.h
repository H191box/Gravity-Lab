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
 *  timer_start — Begin / resume counting
 * ------------------------------------------------------- */
void timer_start(void);

/* -------------------------------------------------------
 *  timer_stop — Pause the timer
 * ------------------------------------------------------- */
void timer_stop(void);

/* -------------------------------------------------------
 *  timer_reset — Reset to zero and stop
 * ------------------------------------------------------- */
void timer_reset(void);

/* -------------------------------------------------------
 *  timer_get_time — Get current elapsed time in centiseconds
 * ------------------------------------------------------- */
u32 timer_get_time(void);

/* -------------------------------------------------------
 *  timer_render — Draw the timer to the HUD
 *  x, y: tile position on text layer
 *  color: text color index
 * ------------------------------------------------------- */
void timer_render(u8 x, u8 y, u8 color);

/* -------------------------------------------------------
 *  timer_save_best — Save best time to SRAM for a level
 *  Returns TRUE if new best time.
 * ------------------------------------------------------- */
bool timer_save_best(u8 level_id, u32 time_cs);

/* -------------------------------------------------------
 *  timer_load_best — Load best time from SRAM
 *  Returns 0 if no time saved.
 * ------------------------------------------------------- */
u32 timer_load_best(u8 level_id);

#endif /* TIMER_DISPLAY_H */