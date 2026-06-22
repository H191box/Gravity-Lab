#ifndef GBA_TYPES_H
#define GBA_TYPES_H

/* ============================================================
 *  GBA Type Definitions — Gravity Lab
 *  Wraps devkitPro's <gba.h> and adds game-specific constants.
 *  devkitPro provides: u8-u32, s8-s32, bool, registers, DMA,
 *  keys, timers, interrupts, OAM, BIOS, ATTR0/1/2, etc.
 * ============================================================ */

/* Use devkitPro's full GBA header */
#include <gba.h>

/* ---- Fixed-point type (NOT in devkitPro) ---- */
typedef s16 fixed;

#ifndef FIXED_SHIFT
#define FIXED_SHIFT   8
#endif
#ifndef FIXED_ONE
#define FIXED_ONE     (1 << FIXED_SHIFT)
#endif
#ifndef FIXED_HALF
#define FIXED_HALF    (1 << (FIXED_SHIFT-1))
#endif
#ifndef FIXED_MASK
#define FIXED_MASK    (FIXED_ONE - 1)
#endif
#ifndef FIXED_PI
#define FIXED_PI      804
#endif
#ifndef FIXED_TWO_PI
#define FIXED_TWO_PI  1608
#endif
#ifndef FIXED_ZERO
#define FIXED_ZERO    0
#endif

/* ---- Utility macros ---- */
#ifndef MAX
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#endif
#ifndef ABS
#define ABS(a)     ((a) < 0 ? -(a) : (a))
#endif
#ifndef CLAMP
#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))
#endif
#ifndef SWAP16
#define SWAP16(x) (((x) >> 8) | ((x) << 8))
#endif

/* ---- Alignment helpers ---- */
#ifndef ALIGN4
#define ALIGN4(x)  (((x) + 3) & ~3)
#endif
#ifndef ALIGN8
#define ALIGN8(x)  (((x) + 7) & ~7)
#endif
#ifndef ALIGN16
#define ALIGN16(x) (((x) + 15) & ~15)
#endif
#ifndef ALIGN32
#define ALIGN32(x) (((x) + 31) & ~31)
#endif

/* ---- Array element count ---- */
#ifndef ARRAY_COUNT
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/* ---- Memory addresses ---- */
#ifndef MEM_EWRAM
#define MEM_EWRAM  0x02000000
#endif
#ifndef MEM_IWRAM
#define MEM_IWRAM  0x03000000
#endif
#ifndef MEM_PAL
#define MEM_PAL    0x05000000
#endif
#ifndef MEM_VRAM
#define MEM_VRAM   0x06000000
#endif
#ifndef MEM_OAM
#define MEM_OAM    0x07000000
#endif
#ifndef MEM_ROM
#define MEM_ROM    0x08000000
#endif
#ifndef MEM_SRAM
#define MEM_SRAM   0x0E000000
#endif

/* ---- Screen dimensions ---- */
#ifndef SCREEN_W
#define SCREEN_W   240
#endif
#ifndef SCREEN_H
#define SCREEN_H   160
#endif

/* ---- Tile dimensions ---- */
#ifndef TILE_SIZE
#define TILE_SIZE  8
#endif

/* ---- OAM ---- */
#ifndef MAX_SPRITES
#define MAX_SPRITES 128
#endif
#ifndef OAM_SIZE
#define OAM_SIZE    (MAX_SPRITES * 4)
#endif

/* ---- Game constants ---- */
#ifndef FPS
#define FPS         60
#endif
#ifndef FRAME_MS
#define FRAME_MS    (1000 / FPS)
#endif
#ifndef FRAME_CS
#define FRAME_CS    1
#endif

/* ============================================================
 *  Display Control (REG_DISPCNT) bit flags
 *  devkitPro uses MODE_0, BG0_ENABLE, OBJ_ENABLE, OBJ_MAP_1D.
 *  We provide our own aliases for readability.
 * ============================================================ */
#ifndef DCNT_MODE0
#define DCNT_MODE0     (0 << 0)
#define DCNT_MODE1     (1 << 0)
#define DCNT_MODE2     (2 << 0)
#define DCNT_MODE3     (3 << 0)
#define DCNT_MODE4     (4 << 0)
#define DCNT_MODE5     (5 << 0)
#define DCNT_BG0       (1 << 8)
#define DCNT_BG1       (1 << 9)
#define DCNT_BG2       (1 << 10)
#define DCNT_BG3       (1 << 11)
#define DCNT_OBJ       (1 << 12)
#define DCNT_OBJ_1D    (1 << 6)
#endif

/* ---- Background Control bits ---- */
#ifndef BG_CNT_PRIORITY
#define BG_CNT_PRIORITY(n)       ((n) << 0)
#endif
#ifndef BG_CNT_CHARBASE
#define BG_CNT_CHARBASE(n)       ((n) << 2)
#endif
#ifndef BG_CNT_SCREENBASE
#define BG_CNT_SCREENBASE(n)     ((n) << 8)
#endif
#ifndef BG_CNT_256COLOR
#define BG_CNT_256COLOR          (1 << 7)
#endif
#ifndef BG_CNT_WRAP
#define BG_CNT_WRAP              (1 << 13)
#endif
#ifndef BG_CNT_SCREENDIM_32x32
#define BG_CNT_SCREENDIM_32x32   (0 << 14)
#endif
#ifndef BG_CNT_SCREENDIM_64x64
#define BG_CNT_SCREENDIM_64x64   (1 << 14)
#endif
#ifndef BG_CNT_SCREENDIM_32x64
#define BG_CNT_SCREENDIM_32x64   (2 << 14)
#endif
#ifndef BG_CNT_SCREENDIM_64x32
#define BG_CNT_SCREENDIM_64x32   (3 << 14)
#endif

/* ---- BG Screen/Char base addresses ---- */
#ifndef BG_SCREENBASE
#define BG_SCREENBASE(n)  (MEM_VRAM + (n) * 0x0800)
#endif
#ifndef BG_CHARBASE
#define BG_CHARBASE(n)    (MEM_VRAM + (n) * 0x4000)
#endif

/* ---- Display Status (REG_DISPSTAT) bits ---- */
#ifndef DSTAT_VBLANK
#define DSTAT_VBLANK   (1 << 0)
#endif
#ifndef DSTAT_HBLANK
#define DSTAT_HBLANK   (1 << 1)
#endif
#ifndef DSTAT_VCOUNT
#define DSTAT_VCOUNT   (1 << 2)
#endif
#ifndef DSTAT_VBL_IRQ
#define DSTAT_VBL_IRQ  (1 << 3)
#endif
#ifndef DSTAT_HBL_IRQ
#define DSTAT_HBL_IRQ  (1 << 4)
#endif
#ifndef DSTAT_VCNT_IRQ
#define DSTAT_VCNT_IRQ (1 << 5)
#endif

/* ---- VBlank flag ---- */
extern volatile u16 vblank_flag;

#endif /* GBA_TYPES_H */
