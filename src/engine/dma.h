#ifndef DMA_H
#define DMA_H

#include "gba_types.h"

/* DMA channel IDs */
#define DMA_CHANNEL0  0
#define DMA_CHANNEL1  1
#define DMA_CHANNEL2  2
#define DMA_CHANNEL3  3

/* DMA transfer control flags */
#define DMA_IMMEDIATE   DMA_NOW
#define DMA_AT_VBLANK   DMA_AT_VBLANK
#define DMA_AT_HBLANK   DMA_AT_HBLANK

/* -------------------------------------------------------
 *  dma_memcpy — Copy data using DMA
 *
 *  dst:  destination address
 *  src:  source address
 *  count: number of UNITS to transfer (words if 32-bit, halfwords if 16-bit)
 *  ctrl:  DMA control flags (DMA_16BIT/DMA_32BIT | timing | DMA_ENABLE)
 *  channel: DMA channel (0-3)
 * ------------------------------------------------------- */
void dma_memcpy(void *dst, const void *src, u32 count, u32 ctrl, int channel);

/* -------------------------------------------------------
 *  dma_memcpy16 — 16-bit DMA copy (shortcut)
 *  count = number of halfwords
 * ------------------------------------------------------- */
void dma_memcpy16(void *dst, const void *src, u32 count);

/* -------------------------------------------------------
 *  dma_memcpy32 — 32-bit DMA copy (shortcut)
 *  count = number of words
 * ------------------------------------------------------- */
void dma_memcpy32(void *dst, const void *src, u32 count);

/* -------------------------------------------------------
 *  dma_memset — Fill memory with a value using DMA
 *
 *  dst:   destination address
 *  val:   16-bit fill value
 *  count: number of halfwords to write
 * ------------------------------------------------------- */
void dma_memset(void *dst, u16 val, u32 count);

/* -------------------------------------------------------
 *  dma_memset32 — Fill memory with a 32-bit value using DMA
 *  count = number of words
 * ------------------------------------------------------- */
void dma_memset32(void *dst, u32 val, u32 count);

/* -------------------------------------------------------
 *  dma_fast_copy — Fast copy using DMA3 (highest priority)
 *  Automatically selects 16-bit or 32-bit based on alignment
 *  count = number of BYTES
 * ------------------------------------------------------- */
void dma_fast_copy(void *dst, const void *src, u32 byte_count);

#endif /* DMA_H */