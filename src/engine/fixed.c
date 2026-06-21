#include "fixed.h"

/* -------------------------------------------------------
 *  Sin / Cos lookup tables (256 entries, 256-degree circle)
 *
 *  Stored as signed bytes representing -1.0 to 1.0
 *  where 127 ≈ 1.0 and -128 ≈ -1.0
 *
 *  sin(angle) where angle 0=0°, 64=90°, 128=180°, 192=270°
 *  Note: In GBA screen coords, Y increases downward, so
 *  sin(0) = 0, sin(64) = -127 (up on screen)
 *
 *  We use the mathematical sin but with Y inverted for screen:
 *  sin_table[i] = round(127 * sin(i * 2π / 256))
 *  But since GBA Y is inverted: sin_screen = -sin_math
 *  cos_table[i] = round(127 * cos(i * 2π / 256))
 * ------------------------------------------------------- */

const s8 sin_table[256] = {
       0,   3,   6,   9,  12,  16,  19,  22,
      25,  28,  31,  34,  37,  40,  43,  46,
      49,  51,  54,  57,  60,  63,  65,  68,
      71,  73,  76,  78,  81,  83,  85,  88,
      90,  92,  94,  96,  98, 100, 102, 104,
     106, 107, 109, 111, 112, 113, 115, 116,
     117, 118, 120, 121, 122, 122, 123, 124,
     125, 125, 126, 126, 127, 127, 127, 127,
     127, 127, 127, 127, 127, 126, 126, 125,
     125, 124, 123, 122, 122, 121, 120, 118,
     117, 116, 115, 113, 112, 111, 109, 107,
     106, 104, 102, 100,  98,  96,  94,  92,
      90,  88,  85,  83,  81,  78,  76,  73,
      71,  68,  65,  63,  60,  57,  54,  51,
      49,  46,  43,  40,  37,  34,  31,  28,
      25,  22,  19,  16,  12,   9,   6,   3,
       0,  -3,  -6,  -9, -12, -16, -19, -22,
     -25, -28, -31, -34, -37, -40, -43, -46,
     -49, -51, -54, -57, -60, -63, -65, -68,
     -71, -73, -76, -78, -81, -83, -85, -88,
     -90, -92, -94, -96, -98,-100,-102,-104,
    -106,-107,-109,-111,-112,-113,-115,-116,
    -117,-118,-120,-121,-122,-122,-123,-124,
    -125,-125,-126,-126,-127,-127,-127,-127,
    -127,-127,-127,-127,-127,-126,-126,-125,
    -125,-124,-123,-122,-122,-121,-120,-118,
    -117,-116,-115,-113,-112,-111,-109,-107,
    -106,-104,-102,-100, -98, -96, -94, -92,
     -90, -88, -85, -83, -81, -78, -76, -73,
     -71, -68, -65, -63, -60, -57, -54, -51,
     -49, -46, -43, -40, -37, -34, -31, -28,
     -25, -22, -19, -16, -12,  -9,  -6,  -3
};

/* cos_table[i] = sin_table[i + 64] (cos = sin shifted by 90°) */
const s8 cos_table[256] = {
     127, 127, 127, 127, 127, 126, 126, 125,
     125, 124, 123, 122, 122, 121, 120, 118,
     117, 116, 115, 113, 112, 111, 109, 107,
     106, 104, 102, 100,  98,  96,  94,  92,
      90,  88,  85,  83,  81,  78,  76,  73,
      71,  68,  65,  63,  60,  57,  54,  51,
      49,  46,  43,  40,  37,  34,  31,  28,
      25,  22,  19,  16,  12,   9,   6,   3,
       0,  -3,  -6,  -9, -12, -16, -19, -22,
     -25, -28, -31, -34, -37, -40, -43, -46,
     -49, -51, -54, -57, -60, -63, -65, -68,
     -71, -73, -76, -78, -81, -83, -85, -88,
     -90, -92, -94, -96, -98,-100,-102,-104,
    -106,-107,-109,-111,-112,-113,-115,-116,
    -117,-118,-120,-121,-122,-122,-123,-124,
    -125,-125,-126,-126,-127,-127,-127,-127,
    -127,-127,-127,-127,-127,-126,-126,-125,
    -125,-124,-123,-122,-122,-121,-120,-118,
    -117,-116,-115,-113,-112,-111,-109,-107,
    -106,-104,-102,-100, -98, -96, -94, -92,
     -90, -88, -85, -83, -81, -78, -76, -73,
     -71, -68, -65, -63, -60, -57, -54, -51,
     -49, -46, -43, -40, -37, -34, -31, -28,
     -25, -22, -19, -16, -12,  -9,  -6,  -3,
       0,   3,   6,   9,  12,  16,  19,  22,
      25,  28,  31,  34,  37,  40,  43,  46,
      49,  51,  54,  57,  60,  63,  65,  68,
      71,  73,  76,  78,  81,  83,  85,  88,
      90,  92,  94,  96,  98, 100, 102, 104,
     106, 107, 109, 111, 112, 113, 115, 116,
     117, 118, 120, 121, 122, 122, 123, 124,
     125, 125, 126, 126, 127, 127, 127, 127
};

/* -------------------------------------------------------
 *  fixed_sin — Look up sine in table, return as 8.8 fixed
 *  Result range: -256 to 256 (representing -1.0 to 1.0)
 * ------------------------------------------------------- */
fixed fixed_sin(u8 angle) {
    /* sin_table stores -128 to 127; shift to 8.8 by multiplying by 2 */
    return (fixed)(sin_table[angle] * 2);
}

/* -------------------------------------------------------
 *  fixed_cos — Look up cosine in table, return as 8.8 fixed
 * ------------------------------------------------------- */
fixed fixed_cos(u8 angle) {
    return (fixed)(cos_table[angle] * 2);
}

/* -------------------------------------------------------
 *  fixed_sqrt — Newton's method square root
 *  Returns sqrt(x) in 8.8 fixed. x must be >= 0.
 * ------------------------------------------------------- */
fixed fixed_sqrt(fixed x) {
    if (x <= 0) return 0;

    s32 val = (s32)x;
    s32 guess = val;
    s32 i;

    /* Newton's method: guess = (guess + val/guess) / 2
     * We work in the integer-scaled domain */
    if (guess < 1) guess = 1;

    /* Scale: since x is in 8.8, sqrt(x) is in 4.4
     * We want result in 8.8, so multiply by 16 after sqrt */
    for (i = 0; i < 12; i++) {
        s32 new_guess = (guess + (val << 8) / guess) / 2;
        if (new_guess >= guess) break;
        guess = new_guess;
    }

    /* Convert 4.4 result to 8.8 by shifting left by 4 */
    return (fixed)(guess >> 4);
}

/* -------------------------------------------------------
 *  fixed_atan2 — Compute angle from (x, y) vector
 *  Returns angle 0-255 where:
 *    0 = right, 64 = up, 128 = left, 192 = down
 *  Uses CORDIC-like approach with lookup refinement.
 * ------------------------------------------------------- */
u8 fixed_atan2(fixed y, fixed x) {
    /* Handle edge cases */
    if (x == 0 && y == 0) return 0;
    if (x == 0) return (y > 0) ? 192 : 64;  /* down : up */
    if (y == 0) return (x > 0) ? 0 : 128;   /* right : left */

    /* Convert to screen-angle (y is inverted on screen) */
    /* Use fixed_div to compute y/x, then approximate atan */
    fixed ratio = fixed_div(ABS(y), ABS(x));

    /* Linear approximation for small angles: atan(r) ≈ r for small r
     * Better: use a piecewise linear approximation
     * atan(r) in radians, then convert to 256-degree: angle = atan * 256 / (2π)
     */
    s32 angle_frac;

    if (ratio >= int_to_fixed(1)) {
        /* ratio >= 1: angle is 45-90 degrees
         * atan(r) = π/2 - atan(1/r)
         */
        fixed inv_ratio = fixed_div(FIXED_ONE, ratio);
        angle_frac = 64 - (fixed_to_int(fixed_mul(inv_ratio, int_to_fixed(41))));
    } else {
        /* ratio < 1: angle is 0-45 degrees
         * atan(r) ≈ r * (π/4) for r in [0,1]
         * In 256-degrees: 0-64 maps to 0-64
         * atan(r)*128/π ≈ r * 41 (approximate)
         */
        angle_frac = fixed_to_int(fixed_mul(ratio, int_to_fixed(41)));
    }

    /* Determine quadrant */
    u8 angle;
    if (x > 0) {
        if (y <= 0) {
            /* Quadrant I (right-up): 0 to 63 */
            angle = (u8)angle_frac;
        } else {
            /* Quadrant IV (right-down): 192 to 255 */
            angle = (u8)(192 + angle_frac);
        }
    } else {
        if (y <= 0) {
            /* Quadrant II (left-up): 64 to 127 */
            angle = (u8)(128 - angle_frac);
        } else {
            /* Quadrant III (left-down): 128 to 191 */
            angle = (u8)(128 + angle_frac);
        }
    }

    return angle;
}