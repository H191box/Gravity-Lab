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

/* ---- Boolean constants (not defined in devkitPro's gba.h) ---- */
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* ---- VRAM base ---- */
#ifndef VRAM_BASE
#define VRAM_BASE  0x06000000
#endif

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

/* ============================================================
 *  DMA Control Flags (devkitPro uses different names)
 *  devkitPro has DMA_ENABLE, DMA_16, DMA_32, DMA_REPEAT, DMA_SRC_INC,
 *  DMA_SRC_DEC, DMA_SRC_FIXED, DMA_DST_INC, DMA_DST_DEC, DMA_DST_FIXED,
 *  DMA_DST_RELOAD, DMA_IMMEDIATE, DMA_AT_VBLANK, DMA_AT_HBLANK.
 *  We provide aliases matching our codebase convention.
 * ============================================================ */
#ifndef DMA_16BIT
#define DMA_16BIT          (0 << 10)
#endif
#ifndef DMA_32BIT
#define DMA_32BIT          (1 << 10)
#endif
#ifndef DMA_NOW
#define DMA_NOW            (0 << 12)
#endif
#ifndef DMA_ENABLE
#define DMA_ENABLE         (1 << 15)
#endif
#ifndef DMA_IRQ_DONE
#define DMA_IRQ_DONE       (1 << 14)
#endif
#ifndef DMA_REPEAT
#define DMA_REPEAT         (1 << 9)
#endif
#ifndef DMA_SRC_INC
#define DMA_SRC_INC        (0 << 7)
#endif
#ifndef DMA_SRC_DEC
#define DMA_SRC_DEC        (1 << 7)
#endif
#ifndef DMA_SRC_FIXED
#define DMA_SRC_FIXED      (2 << 7)
#endif
#ifndef DMA_DST_INC
#define DMA_DST_INC        (0 << 5)
#endif
#ifndef DMA_DST_DEC
#define DMA_DST_DEC        (1 << 5)
#endif
#ifndef DMA_DST_FIXED
#define DMA_DST_FIXED      (2 << 5)
#endif
#ifndef DMA_DST_RELOAD
#define DMA_DST_RELOAD     (3 << 5)
#endif
#ifndef DMA_AT_VBLANK
#define DMA_AT_VBLANK      (1 << 12)
#endif
#ifndef DMA_AT_HBLANK
#define DMA_AT_HBLANK      (2 << 12)
#endif
#ifndef DMA_AT_REFRESH
#define DMA_AT_REFRESH     (3 << 12)
#endif

/* ---- Keypad aliases (devkitPro uses REG_KEYINPUT) ---- */
#ifndef REG_KEYPAD
#define REG_KEYPAD     REG_KEYINPUT
#endif

/* ============================================================
 *  OAM Types — Our game's OAM buffer format
 *  devkitPro has OBJATTR (no .padding field) and obj_affine_t.
 *  We define our own ObjAttr/ObjAffine since code accesses .padding.
 * ============================================================ */
typedef struct {
    u16 attr0;
    u16 attr1;
    u16 attr2;
    u16 padding;
} ObjAttr;

typedef struct {
    u16 pa;
    u16 pb;
    u16 pc;
    u16 pd;
} ObjAffine;

#define OAM_BASE       ((ObjAttr *)0x07000000)

/* ---- OBJ Attribute 0 bits ---- */
#ifndef ATTR0_Y_MASK
#define ATTR0_Y_MASK       0x00FF
#endif
#ifndef ATTR0_ROT_SCALE
#define ATTR0_ROT_SCALE    (1 << 8)
#endif
#ifndef ATTR0_DOUBLE
#define ATTR0_DOUBLE       ATTR0_ROTSCALE   /* same bit, different name */
#endif
#ifndef ATTR0_DISABLE
#define ATTR0_DISABLE      ATTR0_DISABLED   /* same bit, different name */
#endif
#ifndef ATTR0_MODE_NORMAL
#define ATTR0_MODE_NORMAL  (0 << 10)
#endif
#ifndef ATTR0_MODE_TRANS
#define ATTR0_MODE_TRANS   (1 << 10)
#endif
#ifndef ATTR0_MOSAIC
#define ATTR0_MOSAIC       (1 << 12)
#endif
#ifndef ATTR0_COLOR_16
#define ATTR0_COLOR_16     (0 << 13)
#endif
#ifndef ATTR0_COLOR_256
#define ATTR0_COLOR_256    (1 << 13)
#endif
#ifndef ATTR0_SHAPE_SQUARE
#define ATTR0_SHAPE_SQUARE  (0 << 14)
#endif
#ifndef ATTR0_SHAPE_WIDE
#define ATTR0_SHAPE_WIDE    (1 << 14)
#endif
#ifndef ATTR0_SHAPE_TALL
#define ATTR0_SHAPE_TALL    (2 << 14)
#endif

/* ---- OBJ Attribute 1 bits ---- */
#ifndef ATTR1_X_MASK
#define ATTR1_X_MASK       0x01FF
#endif
#ifndef ATTR1_HFLIP
#define ATTR1_HFLIP        ATTR1_FLIP_X   /* devkitPro: ATTR1_FLIP_X */
#endif
#ifndef ATTR1_VFLIP
#define ATTR1_VFLIP        ATTR1_FLIP_Y   /* devkitPro: ATTR1_FLIP_Y */
#endif

/* ---- OBJ Attribute 2 bits ---- */
#ifndef ATTR2_ID_MASK
#define ATTR2_ID_MASK      0x03FF
#endif
#ifndef ATTR2_PALETTE
#define ATTR2_PALETTE(n)   ((n) << 12)
#endif
#ifndef ATTR2_PRIORITY
#define ATTR2_PRIORITY(n)  ((n) << 10)
#endif

/* ---- Interrupt flags (devkitPro uses IRQ_*) ---- */
#ifndef INT_VBLANK
#define INT_VBLANK       IRQ_VBLANK
#endif
#ifndef INT_HBLANK
#define INT_HBLANK       IRQ_HBLANK
#endif
#ifndef INT_VCOUNT
#define INT_VCOUNT       IRQ_VCOUNT
#endif
#ifndef INT_TIMER0
#define INT_TIMER0       IRQ_TIMER0
#endif
#ifndef INT_TIMER1
#define INT_TIMER1       IRQ_TIMER1
#endif
#ifndef INT_TIMER2
#define INT_TIMER2       IRQ_TIMER2
#endif
#ifndef INT_TIMER3
#define INT_TIMER3       IRQ_TIMER3
#endif
#ifndef INT_SERIAL
#define INT_SERIAL       IRQ_SERIAL
#endif
#ifndef INT_DMA0
#define INT_DMA0         IRQ_DMA0
#endif
#ifndef INT_DMA1
#define INT_DMA1         IRQ_DMA1
#endif
#ifndef INT_DMA2
#define INT_DMA2         IRQ_DMA2
#endif
#ifndef INT_DMA3
#define INT_DMA3         IRQ_DMA3
#endif
#ifndef INT_JOYPAD
#define INT_JOYPAD       IRQ_JOYPAD
#endif
#ifndef INT_KEYPAD
#define INT_KEYPAD       IRQ_KEYPAD
#endif

/* ---- OBJ Palette (devkitPro defines OBJ_PALETTE as a debug macro, we override) ---- */
#ifdef OBJ_PALETTE
#undef OBJ_PALETTE
#endif
#define OBJ_PALETTE      ((u16 *)0x05000200)

/* ---- BG Palette ---- */
#ifdef BG_PALETTE
#undef BG_PALETTE
#endif
#define BG_PALETTE       ((u16 *)0x05000000)

/* ---- VBlank flag ---- */
extern volatile u16 vblank_flag;

#endif /* GBA_TYPES_H */
