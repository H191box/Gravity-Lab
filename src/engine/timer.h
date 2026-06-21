#ifndef TIMER_H
#define TIMER_H

#include "gba_types.h"

/* Timer channel IDs */
#define TIMER_0  0
#define TIMER_1  1
#define TIMER_2  2
#define TIMER_3  3

/* -------------------------------------------------------
 *  timer_init — Initialize a timer channel
 *
 *  channel: TIMER_0 through TIMER_3
 *  freq:    TM_FREQ_1, TM_FREQ_64, TM_FREQ_256, TM_FREQ_1024
 *  irq:     TRUE to enable interrupt on overflow
 *  cascade: TRUE to enable cascade mode (counts when prev overflows)
 * ------------------------------------------------------- */
void timer_init(int channel, u16 freq, bool irq, bool cascade);

/* -------------------------------------------------------
 *  timer_start — Start a timer with initial value
 *
 *  channel: TIMER_0 through TIMER_3
 *  value:   Initial counter value (u16)
 * ------------------------------------------------------- */
void timer_start(int channel, u16 value);

/* -------------------------------------------------------
 *  timer_stop — Stop a timer
 * ------------------------------------------------------- */
void timer_stop(int channel);

/* -------------------------------------------------------
 *  timer_read — Read current timer value
 *  Returns the raw 16-bit counter value.
 * ------------------------------------------------------- */
u16 timer_read(int channel);

/* -------------------------------------------------------
 *  timer_set_value — Set timer counter directly
 * ------------------------------------------------------- */
void timer_set_value(int channel, u16 value);

/* -------------------------------------------------------
 *  Game Timer — Uses Timer 0/1 cascade for a millisecond counter
 *  Timer 0: counts at FREQ_256 (65536 ticks = ~1 second)
 *  Timer 1: cascaded, increments when Timer 0 overflows
 *
 *  game_timer_init() — Set up the cascade timers
 *  game_timer_reset() — Reset to zero
 *  game_timer_get_ms() — Get elapsed milliseconds
 *  game_timer_get_cs() — Get elapsed centiseconds (1/100 sec)
 * ------------------------------------------------------- */
void game_timer_init(void);
void game_timer_reset(void);
u32 game_timer_get_ms(void);
u32 game_timer_get_cs(void);

#endif /* TIMER_H */