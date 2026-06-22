#ifndef FIXED_H
#define FIXED_H

#include "gba_types.h"

/* FIXED_SHIFT, FIXED_ONE, FIXED_HALF, FIXED_MASK are defined in gba_types.h */

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

/* Trig tables */
extern const s8 sin_table[256];
extern const s8 cos_table[256];

fixed fixed_sin(u8 angle);
fixed fixed_cos(u8 angle);
fixed fixed_sqrt(fixed x);
u8 fixed_atan2(fixed y, fixed x);

#endif /* FIXED_H */
