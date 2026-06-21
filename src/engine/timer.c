#include "timer.h"
#include "gba_header.h"

/* Timer data register addresses */
static volatile u16 *const tm_data[4] = {
    (volatile u16 *)0x04000100,  /* REG_TM0D */
    (volatile u16 *)0x04000104,  /* REG_TM1D */
    (volatile u16 *)0x04000108,  /* REG_TM2D */
    (volatile u16 *)0x0400010C,  /* REG_TM3D */
};

/* Timer control register addresses */
static volatile u16 *const tm_ctrl[4] = {
    (volatile u16 *)0x04000102,  /* REG_TM0CNT */
    (volatile u16 *)0x04000106,  /* REG_TM1CNT */
    (volatile u16 *)0x0400010A,  /* REG_TM2CNT */
    (volatile u16 *)0x0400010E,  /* REG_TM3CNT */
};

/* -------------------------------------------------------
 *  timer_init — Configure a timer channel
 * ------------------------------------------------------- */
void timer_init(int channel, u16 freq, bool irq, bool cascade) {
    if (channel < 0 || channel > 3) return;

    u16 ctrl = (freq & 0x0003);
    if (irq)     ctrl |= TM_IRQ;
    if (cascade) ctrl |= TM_CASCADE;
    ctrl |= TM_ENABLE;

    /* Set control register (this starts the timer) */
    tm_ctrl[channel][0] = ctrl;
}

/* -------------------------------------------------------
 *  timer_start — Write initial value and start timer
 * ------------------------------------------------------- */
void timer_start(int channel, u16 value) {
    if (channel < 0 || channel > 3) return;

    /* Write value to data register */
    tm_data[channel][0] = value;

    /* Ensure timer is enabled */
    tm_ctrl[channel][0] |= TM_ENABLE;
}

/* -------------------------------------------------------
 *  timer_stop — Disable a timer
 * ------------------------------------------------------- */
void timer_stop(int channel) {
    if (channel < 0 || channel > 3) return;
    tm_ctrl[channel][0] &= ~TM_ENABLE;
}

/* -------------------------------------------------------
 *  timer_read — Read the current 16-bit counter
 * ------------------------------------------------------- */
u16 timer_read(int channel) {
    if (channel < 0 || channel > 3) return 0;
    return tm_data[channel][0];
}

/* -------------------------------------------------------
 *  timer_set_value — Directly set counter value
 * ------------------------------------------------------- */
void timer_set_value(int channel, u16 value) {
    if (channel < 0 || channel > 3) return;
    tm_data[channel][0] = value;
}

/* -------------------------------------------------------
 *  Game Timer Implementation
 *
 *  Timer 0: FREQ_256 = 65,536 Hz → overflows every ~1 second (65536 ticks)
 *  Timer 1: Cascaded → increments once per Timer 0 overflow
 *
 *  Combined: Timer1 << 16 | Timer0 gives ~65,536 µs resolution
 *  For milliseconds: total_ticks / 65.536
 *  For centiseconds: total_ticks / 655.36
 * ------------------------------------------------------- */

/* -------------------------------------------------------
 *  game_timer_init — Start cascaded timer pair
 * ------------------------------------------------------- */
void game_timer_init(void) {
    /* Stop both timers first */
    timer_stop(TIMER_0);
    timer_stop(TIMER_1);

    /* Timer 0: FREQ_256 (65536 Hz), no IRQ, no cascade */
    tm_data[TIMER_0][0] = 0;
    tm_ctrl[TIMER_0][0] = TM_FREQ_256 | TM_ENABLE;

    /* Timer 1: Cascaded, no IRQ */
    tm_data[TIMER_1][0] = 0;
    tm_ctrl[TIMER_1][0] = TM_CASCADE | TM_ENABLE;
}

/* -------------------------------------------------------
 *  game_timer_reset — Reset both timers to zero
 *  We must briefly stop them to avoid race conditions.
 * ------------------------------------------------------- */
void game_timer_reset(void) {
    /* Stop timers */
    timer_stop(TIMER_0);
    timer_stop(TIMER_1);

    /* Reset counters */
    tm_data[TIMER_0][0] = 0;
    tm_data[TIMER_1][0] = 0;

    /* Restart */
    tm_ctrl[TIMER_0][0] = TM_FREQ_256 | TM_ENABLE;
    tm_ctrl[TIMER_1][0] = TM_CASCADE | TM_ENABLE;
}

/* -------------------------------------------------------
 *  game_timer_get_ms — Get elapsed time in milliseconds
 *
 *  Total ticks = (Timer1 << 16) + Timer0
 *  At 65536 Hz, 1 tick = ~15.26 µs
 *  milliseconds = ticks * 1000 / 65536 ≈ ticks / 65.536
 *  Use (ticks * 1000) >> 16 for efficiency
 * ------------------------------------------------------- */
u32 game_timer_get_ms(void) {
    /* Read Timer 0 first (it might overflow between reads) */
    u16 t0 = tm_data[TIMER_0][0];
    u16 t1 = tm_data[TIMER_1][0];

    /* Check if Timer 0 overflowed during read */
    u16 t0_check = tm_data[TIMER_0][0];
    if (t0_check < t0) {
        /* Overflow happened — re-read Timer 1 */
        t1 = tm_data[TIMER_1][0];
        t0 = t0_check;
    }

    u32 total_ticks = ((u32)t1 << 16) | (u32)t0;

    /* Convert to milliseconds: ticks * 1000 / 65536 */
    /* = (ticks * 1000) >> 16 */
    return (total_ticks * 1000) >> 16;
}

/* -------------------------------------------------------
 *  game_timer_get_cs — Get elapsed time in centiseconds (1/100 sec)
 *
 *  centiseconds = ticks * 100 / 65536 ≈ (ticks * 100) >> 16
 * ------------------------------------------------------- */
u32 game_timer_get_cs(void) {
    u16 t0 = tm_data[TIMER_0][0];
    u16 t1 = tm_data[TIMER_1][0];

    u16 t0_check = tm_data[TIMER_0][0];
    if (t0_check < t0) {
        t1 = tm_data[TIMER_1][0];
        t0 = t0_check;
    }

    u32 total_ticks = ((u32)t1 << 16) | (u32)t0;

    /* Convert to centiseconds: ticks * 100 / 65536 */
    return (total_ticks * 100) >> 16;
}