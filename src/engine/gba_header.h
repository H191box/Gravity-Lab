#ifndef GBA_HEADER_H
#define GBA_HEADER_H

#include "gba_types.h"

/* ============================================================
 *  GBA Memory-Mapped I/O Register Definitions
 *  Gravity Lab — Register addresses for GBA hardware
 * ============================================================ */

/* --- Display Control --- */
#define REG_BASE       0x04000000
#define REG_DISPCNT    *(volatile u16 *)0x04000000

/* DISPCNT bits */
#define DCNT_MODE0     (0 << 0)
#define DCNT_MODE1     (1 << 0)
#define DCNT_MODE2     (2 << 0)
#define DCNT_MODE3     (3 << 0)
#define DCNT_MODE4     (4 << 0)
#define DCNT_BG0       (1 << 8)
#define DCNT_BG1       (1 << 9)
#define DCNT_BG2       (1 << 10)
#define DCNT_BG3       (1 << 11)
#define DCNT_OBJ       (1 << 12)
#define DCNT_OBJ_1D    (1 << 6)

/* --- Background Control --- */
#define REG_BG0CNT     *(volatile u16 *)0x04000008
#define REG_BG1CNT     *(volatile u16 *)0x0400000A
#define REG_BG2CNT     *(volatile u16 *)0x0400000C
#define REG_BG3CNT     *(volatile u16 *)0x0400000E

/* BG control bits */
#define BG_CNT_PRIORITY(n)       ((n) << 0)
#define BG_CNT_CHARBASE(n)       ((n) << 2)
#define BG_CNT_SCREENBASE(n)     ((n) << 8)
#define BG_CNT_256COLOR          (1 << 7)
#define BG_CNT_SCREENDIM_32x32   (0 << 14)
#define BG_CNT_SCREENDIM_64x64   (1 << 14)
#define BG_CNT_SCREENDIM_32x64   (2 << 14)
#define BG_CNT_SCREENDIM_64x32   (3 << 14)
#define BG_CNT_WRAP              (1 << 13)

/* BG Screen-Base offsets: each base = 2KB, max 32KB for BG maps */
#define BG_SCREENBASE(n)  (0x06000000 + (n) * 0x0800)
#define BG_CHARBASE(n)    (0x06000000 + (n) * 0x4000)

/* --- Background Scroll Offsets --- */
#define REG_BG0HOFS    *(volatile u16 *)0x04000010
#define REG_BG0VOFS    *(volatile u16 *)0x04000012
#define REG_BG1HOFS    *(volatile u16 *)0x04000014
#define REG_BG1VOFS    *(volatile u16 *)0x04000016
#define REG_BG2HOFS    *(volatile u16 *)0x04000018
#define REG_BG2VOFS    *(volatile u16 *)0x0400001A
#define REG_BG3HOFS    *(volatile u16 *)0x0400001C
#define REG_BG3VOFS    *(volatile u16 *)0x0400001E

/* --- Display Status --- */
#define REG_DISPSTAT   *(volatile u16 *)0x04000004
#define REG_VCOUNT     *(volatile u16 *)0x04000006

/* DISPSTAT bits */
#define DSTAT_VBLANK   (1 << 0)
#define DSTAT_HBLANK   (1 << 1)
#define DSTAT_VCOUNT   (1 << 2)
#define DSTAT_VBL_IRQ  (1 << 3)
#define DSTAT_HBL_IRQ  (1 << 4)
#define DSTAT_VCNT_IRQ (1 << 5)

/* --- Interrupt / Wait State Control --- */
#define REG_IE         *(volatile u16 *)0x04000200
#define REG_IF         *(volatile u16 *)0x04000202
#define REG_IME        *(volatile u16 *)0x04000208

/* Interrupt flags */
#define INT_VBLANK     (1 << 0)
#define INT_HBLANK     (1 << 1)
#define INT_VCOUNT     (1 << 2)
#define INT_TIMER0     (1 << 3)
#define INT_TIMER1     (1 << 4)
#define INT_TIMER2     (1 << 5)
#define INT_TIMER3     (1 << 6)

/* --- DMA Transfer --- */
#define REG_DMA0SAD    *(volatile u32 *)0x040000B0
#define REG_DMA0DAD    *(volatile u32 *)0x040000B4
#define REG_DMA0CNT    *(volatile u16 *)0x040000B8
#define REG_DMA1SAD    *(volatile u32 *)0x040000BC
#define REG_DMA1DAD    *(volatile u32 *)0x040000C0
#define REG_DMA1CNT    *(volatile u16 *)0x040000C4
#define REG_DMA2SAD    *(volatile u32 *)0x040000C8
#define REG_DMA2DAD    *(volatile u32 *)0x040000CC
#define REG_DMA2CNT    *(volatile u16 *)0x040000D0
#define REG_DMA3SAD    *(volatile u32 *)0x040000D4
#define REG_DMA3DAD    *(volatile u32 *)0x040000D8
#define REG_DMA3CNT    *(volatile u16 *)0x040000DC

/* DMA control bits */
#define DMA_ENABLE         (1 << 15)
#define DMA_IRQ_DONE       (1 << 14)
#define DMA_16BIT          (0 << 10)
#define DMA_32BIT          (1 << 10)
#define DMA_REPEAT         (1 << 9)
#define DMA_SRC_INC        (0 << 7)
#define DMA_SRC_DEC        (1 << 7)
#define DMA_SRC_FIXED      (2 << 7)
#define DMA_DST_INC        (0 << 5)
#define DMA_DST_DEC        (1 << 5)
#define DMA_DST_FIXED      (2 << 5)
#define DMA_DST_RELOAD     (3 << 5)

/* DMA timing */
#define DMA_NOW            (0 << 12)
#define DMA_AT_VBLANK      (1 << 12)
#define DMA_AT_HBLANK      (2 << 12)
#define DMA_AT_REFRESH     (3 << 12)

/* --- Keypad Input --- */
#define REG_KEYPAD     *(volatile u16 *)0x04000130

/* Key defines (active low) */
#define KEY_A          (1 << 0)
#define KEY_B          (1 << 1)
#define KEY_SELECT     (1 << 2)
#define KEY_START      (1 << 3)
#define KEY_RIGHT      (1 << 4)
#define KEY_LEFT       (1 << 5)
#define KEY_UP         (1 << 6)
#define KEY_DOWN       (1 << 7)
#define KEY_R          (1 << 8)
#define KEY_L          (1 << 9)

/* --- Timer Registers --- */
#define REG_TM0D       *(volatile u16 *)0x04000100
#define REG_TM0CNT     *(volatile u16 *)0x04000102
#define REG_TM1D       *(volatile u16 *)0x04000104
#define REG_TM1CNT     *(volatile u16 *)0x04000106
#define REG_TM2D       *(volatile u16 *)0x04000108
#define REG_TM2CNT     *(volatile u16 *)0x0400010A
#define REG_TM3D       *(volatile u16 *)0x0400010C
#define REG_TM3CNT     *(volatile u16 *)0x0400010E

/* Timer control bits */
#define TM_ENABLE      (1 << 7)
#define TM_IRQ         (1 << 6)
#define TM_CASCADE     (1 << 2)
#define TM_FREQ_1      (0 << 0)   /* 16.78 MHz / 1 */
#define TM_FREQ_64     (1 << 0)   /* 262.144 kHz */
#define TM_FREQ_256    (2 << 0)   /* 65.536 kHz */
#define TM_FREQ_1024   (3 << 0)   /* 16.384 kHz */

/* --- Sound Control (minimal) --- */
#define REG_SNDSTAT    *(volatile u16 *)0x04000080
#define REG_SNDDMGCNT  *(volatile u16 *)0x04000082
#define REG_SNDDSCNT   *(volatile u16 *)0x04000084

/* --- OAM (Object Attribute Memory) --- */
#define OAM_BASE       ((ObjAttr *)0x07000000)
#define OBJ_PALETTE    ((u16 *)0x05000200)

/* --- VRAM --- */
#define VRAM_BASE      0x06000000
#define BG_PALETTE     ((u16 *)0x05000000)

/* --- SRAM --- */
#define SRAM_BASE      ((u8 *)0x0E000000)

/* --- BIOS calls --- */
typedef void (*VBlankIntrFunc)(void);

/* OAM Attribute structure (4 bytes each, 3 words = 12 bytes per sprite) */
typedef struct {
    u16 attr0;
    u16 attr1;
    u16 attr2;
    u16 padding;
} ObjAttr;

/* OAM Affine parameter (8 bytes per matrix) */
typedef struct {
    u16 pa;  /* x-scale */
    u16 pb;  /* y-shear */
    u16 pc;  /* x-shear */
    u16 pd;  /* y-scale */
} ObjAffine;

/* OBJ Attribute 0 bits */
#define ATTR0_Y_MASK       0x00FF
#define ATTR0_ROT_SCALE    (1 << 8)
#define ATTR0_DISABLE      (1 << 9)
#define ATTR0_DOUBLE       (1 << 9)  /* for affine: double-size mode */
#define ATTR0_MODE_NORMAL  (0 << 10)
#define ATTR0_MODE_TRANS   (1 << 10)
#define ATTR0_MODE_WINDOW  (2 << 10)
#define ATTR0_MOSAIC       (1 << 12)
#define ATTR0_COLOR_16     (0 << 13)
#define ATTR0_COLOR_256    (1 << 13)
#define ATTR0_SHAPE_SQUARE (0 << 14)
#define ATTR0_SHAPE_WIDE   (1 << 14)
#define ATTR0_SHAPE_TALL   (2 << 14)

/* OBJ Attribute 1 bits */
#define ATTR1_X_MASK       0x01FF
#define ATTR1_HFLIP        (1 << 12)
#define ATTR1_VFLIP        (1 << 13)
#define ATTR1_SIZE_8       (0 << 14)
#define ATTR1_SIZE_16      (1 << 14)
#define ATTR1_SIZE_32      (2 << 14)
#define ATTR1_SIZE_64      (3 << 14)

/* OBJ Attribute 2 bits */
#define ATTR2_ID_MASK      0x03FF
#define ATTR2_PALETTE(n)   ((n) << 12)
#define ATTR2_PRIORITY(n)  ((n) << 10)

/* Sprite size table [shape][size] = {width, height} */
#define SPRITE_SIZE_W(shape, size) \
    (((((shape) << 2) | (size)) * 2 + 8) & 0x0F) * ((1 + ((((shape) << 2) | (size)) >> 2)) & 3) + 8)

/* ============================================================
 *  BIOS function prototypes (swi calls)
 * ============================================================ */
void bios_vblank_inthandler(void);
void bios_swi_wait_vblank(void);

/* Inline assembly for SWI calls */
static inline void swi_vblank(void) {
    __asm__ volatile("swi 0x05");
}

/* VBlank interrupt flag */
extern volatile u16 vblank_flag;

#endif /* GBA_HEADER_H */