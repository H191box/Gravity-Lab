#ifndef DMA_H
#define DMA_H

#include "gba_types.h"

/* DMA channel IDs */
#define DMA_CHANNEL0  0
#define DMA_CHANNEL1  1
#define DMA_CHANNEL2  2
#define DMA_CHANNEL3  3

/* Compatibility alias */
#ifndef DMA_IMMEDIATE
#define DMA_IMMEDIATE   DMA_NOW
#endif

/* -------------------------------------------------------
 *  dma_memcpy — Copy data using DMA
 * ------------------------------------------------------- */
void dma_memcpy(void *dst, const void *src, u32 count, u32 ctrl, int channel);
void dma_memcpy16(void *dst, const void *src, u32 count);
void dma_memcpy32(void *dst, const void *src, u32 count);
void dma_memset(void *dst, u16 val, u32 count);
void dma_memset32(void *dst, u32 val, u32 count);
void dma_fast_copy(void *dst, const void *src, u32 byte_count);

#endif /* DMA_H */
