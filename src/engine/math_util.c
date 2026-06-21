#include "math_util.h"

/* -------------------------------------------------------
 *  rect_overlap — AABB collision test
 * ------------------------------------------------------- */
bool rect_overlap(const Rect *a, const Rect *b) {
    if (a->x >= b->x + b->w) return FALSE;
    if (a->x + a->w <= b->x) return FALSE;
    if (a->y >= b->y + b->h) return FALSE;
    if (a->y + a->h <= b->y) return FALSE;
    return TRUE;
}

/* -------------------------------------------------------
 *  point_in_rect — Point-in-rectangle test
 * ------------------------------------------------------- */
bool point_in_rect(s32 px, s32 py, const Rect *r) {
    if (px < r->x) return FALSE;
    if (px >= r->x + (s32)r->w) return FALSE;
    if (py < r->y) return FALSE;
    if (py >= r->y + (s32)r->h) return FALSE;
    return TRUE;
}

/* -------------------------------------------------------
 *  circle_rect_overlap — Circle vs AABB collision
 * ------------------------------------------------------- */
bool circle_rect_overlap(s32 cx, s32 cy, s32 cr, const Rect *r) {
    /* Find closest point on rectangle to circle center */
    s32 closest_x = CLAMP(cx, r->x, r->x + (s32)r->w);
    s32 closest_y = CLAMP(cy, r->y, r->y + (s32)r->h);

    /* Check distance from circle center to closest point */
    s32 dx = cx - closest_x;
    s32 dy = cy - closest_y;

    return (dx * dx + dy * dy) <= (cr * cr);
}

/* -------------------------------------------------------
 *  distance — Fast integer square root using
 *  bit-by-bit method (no floating point)
 * ------------------------------------------------------- */
s32 distance(s32 x1, s32 y1, s32 x2, s32 y2) {
    u32 d = distance_sq(x1, y1, x2, y2);
    if (d == 0) return 0;

    /* Bit-by-bit integer square root */
    u32 result = 0;
    u32 bit = (u32)1 << 30;  /* Start from highest bit */

    while (bit > d) {
        bit >>= 2;
    }

    while (bit != 0) {
        if (d >= result + bit) {
            d -= result + bit;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }
        bit >>= 2;
    }

    return (s32)result;
}

/* -------------------------------------------------------
 *  angle_diff — Shortest angular difference on 256-degree circle
 *  Returns signed value in [-128, 127]
 * ------------------------------------------------------- */
s16 angle_diff(u8 a, u8 b) {
    s16 diff = (s16)(s8)(b - a);
    return diff;
}

/* -------------------------------------------------------
 *  angle_lerp — Interpolate between two angles
 *  t: 0 = from, 256 = to
 * ------------------------------------------------------- */
u8 angle_lerp(u8 from, u8 to, s32 t) {
    s16 diff = angle_diff(from, to);
    s32 result = (s32)from + ((diff * t) >> 8);
    return (u8)(result & 0xFF);
}

/* ============================================================
 *  Pseudo-Random Number Generator (LCG)
 *  Period: 2^32, fast, no floating point
 * ============================================================ */
static u32 rng_state = 12345;

void random_seed(u32 seed) {
    rng_state = seed ? seed : 1;  /* Avoid zero state */
}

u32 random_next(void) {
    /* Numerical Recipes LCG: state = state * 1664525 + 1013904223 */
    rng_state = rng_state * 1664525u + 1013904223u;
    return rng_state;
}

s32 random_range(s32 lo, s32 hi) {
    if (lo >= hi) return lo;
    u32 range = (u32)(hi - lo + 1);
    return lo + (s32)(random_next() % range);
}

/* ============================================================
 *  Screen Shake System
 * ============================================================ */
static u8  shake_intensity = 0;
static u8  shake_duration  = 0;
static u8  shake_timer     = 0;

void shake_start(u8 intensity, u8 duration) {
    shake_intensity = intensity;
    shake_duration  = duration;
    shake_timer     = 0;
}

void shake_update(void) {
    if (shake_timer < shake_duration) {
        shake_timer++;
    }
}

s16 shake_get_x(void) {
    if (shake_timer >= shake_duration) return 0;
    /* Decrease intensity over time */
    u8 remaining = shake_duration - shake_timer;
    u8 strength = (shake_intensity * remaining) / shake_duration;
    return (s16)((random_next() % (strength * 2 + 1)) - (s32)strength);
}

s16 shake_get_y(void) {
    if (shake_timer >= shake_duration) return 0;
    u8 remaining = shake_duration - shake_timer;
    u8 strength = (shake_intensity * remaining) / shake_duration;
    return (s16)((random_next() % (strength * 2 + 1)) - (s32)strength);
}