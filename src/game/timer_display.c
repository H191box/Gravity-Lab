#include "timer_display.h"
#include "text_renderer.h"
#include "timer.h"
#include "save.h"

/* Game timer instance */
static GameTimer game_timer;

/* Offset: centiseconds when timer was started/resumed
 * We use the hardware timer as a reference and compute delta */
static u32 hw_timer_offset = 0;

/* Saved centisecond count when we last paused */
static u32 saved_cs = 0;

/* -------------------------------------------------------
 *  timer_display_init — Set up timer
 * ------------------------------------------------------- */
void timer_display_init(void) {
    game_timer.centiseconds = 0;
    game_timer.running = FALSE;
    game_timer.started = FALSE;
    saved_cs = 0;
    game_timer_init();
}

/* -------------------------------------------------------
 *  timer_start — Begin counting
 * ------------------------------------------------------- */
void timer_start(void) {
    if (game_timer.running) return;

    game_timer.running = TRUE;
    game_timer.started = TRUE;

    /* Reset hardware timer and record offset */
    game_timer_reset();
    hw_timer_offset = game_timer_get_cs();
}

/* -------------------------------------------------------
 *  timer_stop — Pause the timer
 * ------------------------------------------------------- */
void timer_stop(void) {
    if (!game_timer.running) return;

    /* Save current elapsed time */
    u32 hw_now = game_timer_get_cs();
    u32 delta = hw_now - hw_timer_offset;
    game_timer.centiseconds = saved_cs + delta;

    game_timer.running = FALSE;
    saved_cs = game_timer.centiseconds;
}

/* -------------------------------------------------------
 *  timer_reset — Reset timer to zero
 * ------------------------------------------------------- */
void timer_reset(void) {
    game_timer.centiseconds = 0;
    game_timer.running = FALSE;
    game_timer.started = FALSE;
    saved_cs = 0;
    game_timer_reset();
}

/* -------------------------------------------------------
 *  timer_get_time — Get elapsed centiseconds
 * ------------------------------------------------------- */
u32 timer_get_time(void) {
    if (!game_timer.started) return 0;

    if (game_timer.running) {
        u32 hw_now = game_timer_get_cs();
        u32 delta = hw_now - hw_timer_offset;
        return saved_cs + delta;
    } else {
        return game_timer.centiseconds;
    }
}

/* -------------------------------------------------------
 *  timer_render — Draw time as MM:SS.CC
 * ------------------------------------------------------- */
void timer_render(u8 x, u8 y, u8 color) {
    u32 cs = timer_get_time();
    text_draw_time(x, y, cs, color);
}

/* -------------------------------------------------------
 *  timer_save_best — Save to SRAM if it's a new best
 * ------------------------------------------------------- */
bool timer_save_best(u8 level_id, u32 time_cs) {
    u32 old_best = timer_load_best(level_id);

    if (old_best == 0 || time_cs < old_best) {
        /* New best time! */
        SaveData save;
        save_read(&save);

        if (level_id < MAX_LEVELS) {
            save.best_times[level_id] = time_cs;

            /* Mark level as completed (unlock next) */
            if (level_id + 1 < MAX_LEVELS) {
                save.unlocked |= (1u << (level_id + 1));
            }

            save_write(&save);
            return TRUE;
        }
    }
    return FALSE;
}

/* -------------------------------------------------------
 *  timer_load_best — Read best time from SRAM
 * ------------------------------------------------------- */
u32 timer_load_best(u8 level_id) {
    SaveData save;
    if (!save_read(&save)) return 0;
    if (level_id >= MAX_LEVELS) return 0;
    return save.best_times[level_id];
}