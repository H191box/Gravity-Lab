#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include "gba_types.h"
#include "fixed.h"

/* Axis-Aligned Bounding Box collision rectangle */
typedef struct {
    s16 x;      /* Top-left x in pixels */
    s16 y;      /* Top-left y in pixels */
    u16 w;      /* Width in pixels */
    u16 h;      /* Height in pixels */
} Rect;

/* 2D point */
typedef struct {
    s32 x;
    s32 y;
} Point;

/* Fixed-point 2D vector */
typedef struct {
    fixed x;
    fixed y;
} FVec2;

/* -------------------------------------------------------
 *  clamp — Restrict value to [lo, hi] range
 * ------------------------------------------------------- */
static inline s32 clamp(s32 val, s32 lo, s32 hi) {
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

/* -------------------------------------------------------
 *  lerp — Linear interpolation
 *  t: 0.0 = returns a, 1.0 = returns b
 *  Uses fixed-point: t is in 0-256 range
 * ------------------------------------------------------- */
static inline s32 lerp(s32 a, s32 b, s32 t) {
    /* t in 0..256 */
    return a + (((b - a) * t) >> 8);
}

/* Fixed-point lerp */
static inline fixed fixed_lerp(fixed a, fixed b, fixed t) {
    return fixed_add(a, fixed_mul(fixed_sub(b, a), t));
}

/* -------------------------------------------------------
 *  rect_overlap — Test if two rectangles overlap
 *  Returns TRUE if they intersect
 * ------------------------------------------------------- */
bool rect_overlap(const Rect *a, const Rect *b);

/* -------------------------------------------------------
 *  point_in_rect — Test if a point is inside a rectangle
 * ------------------------------------------------------- */
bool point_in_rect(s32 px, s32 py, const Rect *r);

/* -------------------------------------------------------
 *  circle_rect_overlap — Test circle vs rectangle collision
 *  cx, cy: circle center (integer pixels)
 *  cr: circle radius
 *  r: rectangle
 * ------------------------------------------------------- */
bool circle_rect_overlap(s32 cx, s32 cy, s32 cr, const Rect *r);

/* -------------------------------------------------------
 *  distance_sq — Squared distance between two points
 * ------------------------------------------------------- */
static inline u32 distance_sq(s32 x1, s32 y1, s32 x2, s32 y2) {
    s32 dx = x2 - x1;
    s32 dy = y2 - y1;
    return (u32)(dx * dx + dy * dy);
}

/* -------------------------------------------------------
 *  distance — Approximate distance (fast integer sqrt)
 * ------------------------------------------------------- */
s32 distance(s32 x1, s32 y1, s32 x2, s32 y2);

/* -------------------------------------------------------
 *  angle_diff — Shortest angular difference (256-degree circle)
 *  Returns signed difference in range [-128, 127]
 * ------------------------------------------------------- */
s16 angle_diff(u8 a, u8 b);

/* -------------------------------------------------------
 *  angle_lerp — Interpolate between two angles
 *  Returns angle in 0-255 range
 * ------------------------------------------------------- */
u8 angle_lerp(u8 from, u8 to, s32 t);

/* Random number generator (simple LCG) */
void random_seed(u32 seed);
u32 random_next(void);
s32 random_range(s32 lo, s32 hi);

/* Screen shake support */
void shake_start(u8 intensity, u8 duration);
void shake_update(void);
s16 shake_get_x(void);
s16 shake_get_y(void);

#endif /* MATH_UTIL_H */