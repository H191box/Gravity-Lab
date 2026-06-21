#ifndef GBA_TYPES_H
#define GBA_TYPES_H

/* ============================================================
 *  GBA Type Definitions
 *  Gravity Lab — Basic types for ARM7TDMI target
 * ============================================================ */

/* Unsigned integer types */
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;

/* Signed integer types */
typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;

/* Boolean type */
typedef enum { FALSE = 0, TRUE = 1 } bool;

/* Fixed-point type: 8.8 format (8 integer bits, 8 fractional bits)
 * Range: -128.0 to ~127.996
 * Precision: ~0.004
 * Usage: fixed f = int_to_fixed(3);  // f = 3.0
 *        fixed half = fixed_div(f, int_to_fixed(2));  // 1.5
 */
typedef s16 fixed;

#define FIXED_SHIFT   8
#define FIXED_ONE     (1 << FIXED_SHIFT)    /* 1.0 in fixed-point = 256 */
#define FIXED_HALF    (1 << (FIXED_SHIFT-1)) /* 0.5 in fixed-point = 128 */
#define FIXED_MASK    (FIXED_ONE - 1)

/* Fixed-point constants */
#define FIXED_PI      804     /* ~3.14159 in 8.8 */
#define FIXED_TWO_PI  1608    /* ~6.28318 in 8.8 */
#define FIXED_ZERO    0

/* NULL definition */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* Maximum/Minimum macros */
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#define ABS(a)     ((a) < 0 ? -(a) : (a))
#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))

/* Byte swapping for 16-bit */
#define SWAP16(x) (((x) >> 8) | ((x) << 8))

/* Alignment helpers */
#define ALIGN4(x)  (((x) + 3) & ~3)
#define ALIGN8(x)  (((x) + 7) & ~7)
#define ALIGN16(x) (((x) + 15) & ~15)
#define ALIGN32(x) (((x) + 31) & ~31)

/* Number of elements in an array */
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Useful memory addresses */
#define MEM_EWRAM  0x02000000  /* 256KB External Work RAM */
#define MEM_IWRAM  0x03000000  /* 32KB Internal Work RAM */
#define MEM_IO     0x04000000  /* I/O Registers */
#define MEM_PAL    0x05000000  /* Palette RAM (1KB) */
#define MEM_VRAM   0x06000000  /* Video RAM (96KB) */
#define MEM_OAM    0x07000000  /* OAM (1KB) */
#define MEM_ROM    0x08000000  /* Game Pak ROM (max 32MB) */
#define MEM_SRAM   0x0E000000  /* SRAM (64KB) */

/* Screen dimensions */
#define SCREEN_W   240
#define SCREEN_H   160

/* Tile dimensions */
#define TILE_SIZE  8

/* Maximum OAM entries */
#define MAX_SPRITES 128

/* Total OAM memory in bytes */
#define OAM_SIZE    (MAX_SPRITES * sizeof(((ObjAttr*)0)->attr0) * 3)

/* Game constants */
#define FPS         60
#define FRAME_MS    (1000 / FPS)
#define FRAME_CS    1  /* 1 centisecond per frame at 60fps */

#endif /* GBA_TYPES_H */