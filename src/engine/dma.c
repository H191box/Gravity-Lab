#include "dma.h"
#include "gba_header.h"

/* DMA register arrays for indexed access */
static volatile u32 *const dma_src[4] = {
    (volatile u32 *)0x040000B0,  /* REG_DMA0SAD */
    (volatile u32 *)0x040000BC,  /* REG_DMA1SAD */
    (volatile u32 *)0x040000C8,  /* REG_DMA2SAD */
    (volatile u32 *)0x040000D4,  /* REG_DMA3SAD */
};

static volatile u32 *const dma_dst[4] = {
    (volatile u32 *)0x040000B4,  /* REG_DMA0DAD */
    (volatile u32 *)0x040000C0,  /* REG_DMA1DAD */
    (volatile u32 *)0x040000CC,  /* REG_DMA2DAD */
    (volatile u32 *)0x040000D8,  /* REG_DMA3DAD */
};

/* DMA control register addresses (lower 16 = count, upper 16 = flags) */
static volatile u16 *const dma_cnt[4] = {
    (volatile u16 *)0x040000B8,  /* REG_DMA0CNT_L */
    (volatile u16 *)0x040000C4,  /* REG_DMA1CNT_L */
    (volatile u16 *)0x040000D0,  /* REG_DMA2CNT_L */
    (volatile u16 *)0x040000DC,  /* REG_DMA3CNT_L */
};

/* -------------------------------------------------------
 *  dma_memcpy — General-purpose DMA transfer
 * ------------------------------------------------------- */
void dma_memcpy(void *dst, const void *src, u32 count, u32 ctrl, int channel) {
    if (channel < 0 || channel > 3) return;

    /* Set source and destination */
    dma_src[channel][0] = (u32)src;
    dma_dst[channel][0] = (u32)dst;

    /* Write count to lower 16 bits (DMAxCNT_L), control+enable to upper 16 bits (DMAxCNT_H) */
    dma_cnt[channel][0] = (u16)(count & 0xFFFF);
    dma_cnt[channel][1] = (u16)ctrl | DMA_ENABLE;
}

/* -------------------------------------------------------
 *  dma_memcpy16 — 16-bit DMA copy (immediate timing)
 * ------------------------------------------------------- */
void dma_memcpy16(void *dst, const void *src, u32 count) {
    dma_memcpy(dst, src, count,
               DMA_16BIT | DMA_NOW | DMA_SRC_INC | DMA_DST_INC,
               DMA_CHANNEL3);
}

/* -------------------------------------------------------
 *  dma_memcpy32 — 32-bit DMA copy (immediate timing)
 * ------------------------------------------------------- */
void dma_memcpy32(void *dst, const void *src, u32 count) {
    dma_memcpy(dst, src, count,
               DMA_32BIT | DMA_NOW | DMA_SRC_INC | DMA_DST_INC,
               DMA_CHANNEL3);
}

/* -------------------------------------------------------
 *  dma_memset — Fill with 16-bit value via DMA
 *  Uses DMA3 with source address fixed (points to a temp holding
 *  the fill value, but GBA DMA3 actually has a special behavior:
 *  for DMA fill, we use the source as the value itself).
 *
 *  On GBA, DMA channel 3 fill:
 *  - Set destination
 *  - Set count
 *  - Set DMA_DST_INC | DMA_SRC_FIXED | DMA_16BIT | DMA_ENABLE
 *  - Source register holds the fill value
 * ------------------------------------------------------- */
void dma_memset(void *dst, u16 val, u32 count) {
    dma_src[DMA_CHANNEL3][0] = (u32)val;
    dma_dst[DMA_CHANNEL3][0] = (u32)dst;
    dma_cnt[DMA_CHANNEL3][0] = (u16)count;
    dma_cnt[DMA_CHANNEL3][1] = DMA_16BIT | DMA_DST_INC | DMA_SRC_FIXED | DMA_ENABLE;
}

/* -------------------------------------------------------
 *  dma_memset32 — Fill with 32-bit value via DMA
 * ------------------------------------------------------- */
void dma_memset32(void *dst, u32 val, u32 count) {
    dma_src[DMA_CHANNEL3][0] = val;
    dma_dst[DMA_CHANNEL3][0] = (u32)dst;
    dma_cnt[DMA_CHANNEL3][0] = (u16)count;
    dma_cnt[DMA_CHANNEL3][1] = DMA_32BIT | DMA_DST_INC | DMA_SRC_FIXED | DMA_ENABLE;
}

/* -------------------------------------------------------
 *  dma_fast_copy — Auto-detect best transfer mode
 *  Uses DMA3 for highest priority transfers.
 *  byte_count must be a multiple of 2 or 4.
 * ------------------------------------------------------- */
void dma_fast_copy(void *dst, const void *src, u32 byte_count) {
    if (byte_count & 3) {
        /* Not 4-byte aligned — use 16-bit transfer */
        dma_memcpy16(dst, src, byte_count / 2);
    } else {
        /* 4-byte aligned — use faster 32-bit transfer */
        dma_memcpy32(dst, src, byte_count / 4);
    }
}