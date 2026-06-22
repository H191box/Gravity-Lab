#include "sprite_manager.h"
#include "dma.h"
#include "fixed.h"

/* Pool of 128 sprite slots */
static GameSprite sprites[MAX_SPRITES];

/* OAM buffer — we write here, then DMA to hardware OAM at VBlank */
static ObjAttr  oam_buffer[MAX_SPRITES];
static ObjAffine oam_affine[32];  /* 32 affine matrices */

/* -------------------------------------------------------
 *  Helper: Determine OAM shape/size encoding from pixel dimensions
 *
 *  Shape/Size table:
 *  Square: 8x8(0), 16x16(1), 32x32(2), 64x64(3)
 *  Wide:   16x8(0), 32x8(1), 32x16(2), 64x32(3)
 *  Tall:   8x16(0), 8x32(1), 16x32(2), 32x64(3)
 * ------------------------------------------------------- */
static u8 get_shape_size(u8 w, u8 h) {
    if (w == h) {
        /* Square shape */
        switch (w) {
            case 8:  return (0 << 2) | 0;  /* Square, 8x8 */
            case 16: return (0 << 2) | 1;  /* Square, 16x16 */
            case 32: return (0 << 2) | 2;  /* Square, 32x32 */
            case 64: return (0 << 2) | 3;  /* Square, 64x64 */
            default: return (0 << 2) | 0;
        }
    } else if (w > h) {
        /* Wide shape */
        switch (w) {
            case 16: return (1 << 2) | 0;  /* Wide, 16x8 */
            case 32:
                if (h == 8)  return (1 << 2) | 1;  /* Wide, 32x8 */
                if (h == 16) return (1 << 2) | 2;  /* Wide, 32x16 */
                return (1 << 2) | 1;
            case 64: return (1 << 2) | 3;  /* Wide, 64x32 */
            default: return (1 << 2) | 0;
        }
    } else {
        /* Tall shape */
        switch (h) {
            case 16: return (2 << 2) | 0;  /* Tall, 8x16 */
            case 32:
                if (w == 8)  return (2 << 2) | 1;  /* Tall, 8x32 */
                if (w == 16) return (2 << 2) | 2;  /* Tall, 16x32 */
                return (2 << 2) | 1;
            case 64: return (2 << 2) | 3;  /* Tall, 32x64 */
            default: return (2 << 2) | 0;
        }
    }
}

/* -------------------------------------------------------
 *  sprite_init — Clear all sprite slots and OAM
 * ------------------------------------------------------- */
void sprite_init(void) {
    int i;
    for (i = 0; i < MAX_SPRITES; i++) {
        sprites[i].state = SPRITE_FREE;
        sprites[i].flags = 0;
        sprites[i].x = 0;
        sprites[i].y = SCREEN_H;  /* Off-screen */
        sprites[i].width = 8;
        sprites[i].height = 8;
        sprites[i].tile_id = 0;
        sprites[i].attr2_extra = 0;
        sprites[i].shape_size = 0;
        sprites[i].affine_index = 0;
    }

    /* Initialize OAM buffer — all sprites hidden */
    for (i = 0; i < MAX_SPRITES; i++) {
        oam_buffer[i].attr0 = ATTR0_Y_MASK;  /* y=160 (off-screen) */
        oam_buffer[i].attr1 = 0;
        oam_buffer[i].attr2 = 0;
        oam_buffer[i].padding = 0;
    }

    /* Initialize affine matrices to identity */
    for (i = 0; i < 32; i++) {
        oam_affine[i].pa = FIXED_ONE;  /* 1.0 */
        oam_affine[i].pb = 0;
        oam_affine[i].pc = 0;
        oam_affine[i].pd = FIXED_ONE;  /* 1.0 */
    }

    /* Copy OAM to hardware immediately */
    /* Each ObjAttr is 4 halfwords (8 bytes): attr0, attr1, attr2, padding */
    dma_memcpy16((void *)OAM_BASE, oam_buffer, MAX_SPRITES * 4);
}

/* -------------------------------------------------------
 *  sprite_alloc — Find a free slot and configure it
 * ------------------------------------------------------- */
int sprite_alloc(u8 width, u8 height, u16 tile_id, u8 palette, u8 priority) {
    int i;
    for (i = 0; i < MAX_SPRITES; i++) {
        if (sprites[i].state == SPRITE_FREE) {
            sprites[i].state = SPRITE_ACTIVE;
            sprites[i].flags = SPRITE_FLAG_VISIBLE;
            sprites[i].tile_id = tile_id;
            sprites[i].width = width;
            sprites[i].height = height;
            sprites[i].shape_size = get_shape_size(width, height);
            sprites[i].x = 0;
            sprites[i].y = SCREEN_H;  /* Start off-screen */
            sprites[i].affine_index = 0;

            /* Build attr2 extra bits: palette (12-15) and priority (10-11) */
            sprites[i].attr2_extra = ATTR2_PALETTE(palette) | ATTR2_PRIORITY(priority);

            return i;
        }
    }
    return -1;  /* No free slots */
}

/* -------------------------------------------------------
 *  sprite_free — Release a sprite
 * ------------------------------------------------------- */
void sprite_free(int slot) {
    if (slot < 0 || slot >= MAX_SPRITES) return;
    sprites[slot].state = SPRITE_FREE;
    sprites[slot].flags = 0;
    sprites[slot].y = SCREEN_H;  /* Move off-screen */

    /* Immediately hide in OAM buffer */
    oam_buffer[slot].attr0 = ATTR0_Y_MASK;
}

/* -------------------------------------------------------
 *  sprite_set_pos — Set screen position
 * ------------------------------------------------------- */
void sprite_set_pos(int slot, s16 x, s16 y) {
    if (slot < 0 || slot >= MAX_SPRITES) return;
    sprites[slot].x = x;
    sprites[slot].y = y;
}

/* -------------------------------------------------------
 *  sprite_set_visible — Toggle visibility
 * ------------------------------------------------------- */
void sprite_set_visible(int slot, bool visible) {
    if (slot < 0 || slot >= MAX_SPRITES) return;
    if (visible) {
        sprites[slot].flags |= SPRITE_FLAG_VISIBLE;
    } else {
        sprites[slot].flags &= ~SPRITE_FLAG_VISIBLE;
    }
}

/* -------------------------------------------------------
 *  sprite_set_tile — Change tile index (for animation)
 * ------------------------------------------------------- */
void sprite_set_tile(int slot, u16 tile_id) {
    if (slot < 0 || slot >= MAX_SPRITES) return;
    sprites[slot].tile_id = tile_id;
}

/* -------------------------------------------------------
 *  sprite_set_flip — Set flip flags
 * ------------------------------------------------------- */
void sprite_set_flip(int slot, bool hflip, bool vflip) {
    if (slot < 0 || slot >= MAX_SPRITES) return;
    if (hflip) sprites[slot].flags |= SPRITE_FLAG_HFLIP;
    else       sprites[slot].flags &= ~SPRITE_FLAG_HFLIP;

    if (vflip) sprites[slot].flags |= SPRITE_FLAG_VFLIP;
    else       sprites[slot].flags &= ~SPRITE_FLAG_VFLIP;
}

/* -------------------------------------------------------
 *  sprite_set_affine — Set rotation/scale transform
 *  Uses the pre-computed sin/cos tables.
 *
 *  The 2x2 affine matrix for rotation + scale:
 *    pa = cos(a) * sx
 *    pb = -sin(a) * sy
 *    pc = sin(a) * sx
 *    pd = cos(a) * sy
 *
 *  All values in 8.8 fixed-point.
 * ------------------------------------------------------- */
void sprite_set_affine(int slot, u8 angle, fixed sx, fixed sy) {
    if (slot < 0 || slot >= MAX_SPRITES) return;

    /* Mark as affine sprite */
    sprites[slot].flags |= SPRITE_FLAG_AFFINE;

    /* Compute rotation matrix */
    s8 c = cos_table[angle & 0xFF];
    s8 s = sin_table[angle & 0xFF];

    /* Scale sin/cos from [-127,127] to 8.8 fixed [-256,256] */
    fixed cos_val = (fixed)(c * 2);
    fixed sin_val = (fixed)(s * 2);

    /* Apply scale */
    fixed pa = fixed_mul(cos_val, sx);
    fixed pb = fixed_mul(-sin_val, sy);
    fixed pc = fixed_mul(sin_val, sx);
    fixed pd = fixed_mul(cos_val, sy);

    /* Write to affine matrix */
    u8 idx = sprites[slot].affine_index;
    oam_affine[idx].pa = (u16)pa;
    oam_affine[idx].pb = (u16)pb;
    oam_affine[idx].pc = (u16)pc;
    oam_affine[idx].pd = (u16)pd;
}

/* -------------------------------------------------------
 *  sprite_set_priority — Change BG priority
 * ------------------------------------------------------- */
void sprite_set_priority(int slot, u8 priority) {
    if (slot < 0 || slot >= MAX_SPRITES) return;
    priority &= 3;
    sprites[slot].attr2_extra &= ~ATTR2_PRIORITY(3);
    sprites[slot].attr2_extra |= ATTR2_PRIORITY(priority);
}

/* -------------------------------------------------------
 *  sprite_update_all — Flush sprite data to OAM
 *  This is the main render step for sprites.
 *  Called once per frame after all updates.
 * ------------------------------------------------------- */
void sprite_update_all(void) {
    int i;

    /* Write affine matrices to OAM first (they come before sprite attrs) */
    /* In GBA OAM layout: 32 affine matrices (32*4 words), then 128 sprite attrs (128*3 words) */
    /* We DMA the whole thing: affine first, then sprites */
    for (i = 0; i < MAX_SPRITES; i++) {
        GameSprite *sp = &sprites[i];

        if (sp->state != SPRITE_ACTIVE) {
            /* Disabled sprite */
            oam_buffer[i].attr0 = ATTR0_Y_MASK;  /* y=160, disabled */
            oam_buffer[i].attr1 = 0;
            oam_buffer[i].attr2 = 0;
            oam_buffer[i].padding = 0;
            continue;
        }

        /* ATTR0: Y position, mode, shape */
        u16 attr0 = (u16)(sp->y & ATTR0_Y_MASK);
        if (sp->flags & SPRITE_FLAG_AFFINE) {
            attr0 |= ATTR0_ROT_SCALE;
            if (sp->flags & SPRITE_FLAG_SIZE_DBL) {
                attr0 |= ATTR0_DOUBLE;  /* Double-size mode */
            }
        } else {
            attr0 |= ATTR0_DISABLE;  /* Normal mode = !rot_scale, but bit 9 = double for affine.
                                        For non-affine, just ensure rot_scale bit is 0. */
            /* Actually, for non-affine, bit 9 should be 0. ATTR0_DISABLE is (1<<9).
               We should NOT set ATTR0_DISABLE for visible sprites.
               Let me fix: */
            attr0 &= ~ATTR0_DISABLE;  /* Clear bit 9 for normal sprites */
        }
        attr0 |= ATTR0_COLOR_16;  /* 16-color (4bpp) sprites */

        /* Shape from shape_size */
        attr0 |= ((sp->shape_size >> 2) & 3) << 14;

        if (sp->flags & SPRITE_FLAG_MOSAIC) {
            attr0 |= ATTR0_MOSAIC;
        }

        /* ATTR1: X position, flip, size */
        u16 attr1 = (u16)(sp->x & ATTR1_X_MASK);
        attr1 |= ((sp->shape_size & 3) << 14);  /* Size */

        if (!(sp->flags & SPRITE_FLAG_AFFINE)) {
            if (sp->flags & SPRITE_FLAG_HFLIP) attr1 |= ATTR1_HFLIP;
            if (sp->flags & SPRITE_FLAG_VFLIP) attr1 |= ATTR1_VFLIP;
        }

        /* ATTR2: Tile ID, palette, priority */
        u16 attr2 = sp->tile_id & ATTR2_ID_MASK;
        attr2 |= sp->attr2_extra;

        oam_buffer[i].attr0 = attr0;
        oam_buffer[i].attr1 = attr1;
        oam_buffer[i].attr2 = attr2;
        oam_buffer[i].padding = 0;
    }

    /* Copy affine matrices and sprite attributes to OAM via DMA
     * GBA OAM at 0x07000000, total 1024 bytes:
     *   - 32 affine matrices: 32 * 4 halfwords = 256 bytes (0x000-0x0FF)
     *   - 128 sprite attrs: 128 * 3 halfwords = 768 bytes (0x100-0x3FF)
     */
    dma_memcpy16((void *)0x07000000, (const u16 *)oam_affine, 32 * 4);  /* 128 halfwords */
    dma_memcpy16((void *)0x07000100, (const u16 *)oam_buffer, MAX_SPRITES * 4);  /* 512 halfwords */
}

/* -------------------------------------------------------
 *  sprite_clear_all — Hide every sprite immediately
 * ------------------------------------------------------- */
void sprite_clear_all(void) {
    int i;
    for (i = 0; i < MAX_SPRITES; i++) {
        sprites[i].state = SPRITE_FREE;
        oam_buffer[i].attr0 = ATTR0_Y_MASK;
        oam_buffer[i].attr1 = 0;
        oam_buffer[i].attr2 = 0;
        oam_buffer[i].padding = 0;
    }
    dma_memcpy16((void *)OAM_BASE, oam_buffer, MAX_SPRITES * 4);
}

/* -------------------------------------------------------
 *  sprite_get — Get sprite info pointer
 * ------------------------------------------------------- */
GameSprite *sprite_get(int slot) {
    if (slot < 0 || slot >= MAX_SPRITES) return (GameSprite *)NULL;
    return &sprites[slot];
}