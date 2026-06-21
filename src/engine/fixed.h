#ifndef FIXED_H
#define FIXED_H

#include "gba_types.h"

/* Fixed-point format: 8.8 (s16 with 8 fractional bits)
 * Range: -128.0 to 127.996
 * 1.0 = 256 = FIXED_ONE
 */

#define FIXED_SHIFT   8
#define FIXED_ONE     256
#define FIXED_HALF    128
#define FIXED_MASK    255

/* Conversion */
static inline fixed int_to_fixed(s32 i) { return (fixed)(i << FIXED_SHIFT); }
static inline s32   fixed_to_int(fixed f) { return (s32)(f >> FIXED_SHIFT); }
static inline fixed fixed_from_raw(s16 v) { return v; }

/* Basic arithmetic */
static inline fixed fixed_add(fixed a, fixed b) { return (fixed)(a + b); }
static inline fixed fixed_sub(fixed a, fixed b) { return (fixed)(a - b); }
static inline fixed fixed_neg(fixed a) { return (fixed)(-a); }
static inline fixed fixed_abs(fixed a) { return (fixed)(a < 0 ? -a : a); }

/* Multiplication: (a * b) >> 8 */
static inline fixed fixed_mul(fixed a, fixed b) {
    s32 result = ((s32)a * (s32)b) >> FIXED_SHIFT;
    return (fixed)result;
}

/* Division: (a << 8) / b */
static inline fixed fixed_div(fixed a, fixed b) {
    if (b == 0) return 0;
    s32 result = ((s32)a << FIXED_SHIFT) / (s32)b;
    return (fixed)result;
}

/* -------------------------------------------------------
 *  Trigonometry — 256-degree circle
 *  0 = right (0°), 64 = up (90°), 128 = left (180°), 192 = down (270°)
 *  Values in 8.8 fixed-point, range [-1.0, 1.0] → [-256, 256]
 *
 *  Tables generated from: sin(angle * 2π / 256)
 * ------------------------------------------------------- */

/* sin table: 256 entries covering full circle */
extern const s8 sin_table[256];
extern const s8 cos_table[256];

/* Fixed-point sin/cos: returns value in [-1.0, 1.0] as fixed (8.8) */
fixed fixed_sin(u8 angle);
fixed fixed_cos(u8 angle);

/* Fixed-point sqrt using Newton's method */
fixed fixed_sqrt(fixed x);

/* Fixed-point atan2 — returns angle in 0-255 */
u8 fixed_atan2(fixed y, fixed x);

#endif /* FIXED_H */