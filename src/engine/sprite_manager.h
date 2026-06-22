#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "gba_types.h"

/* Sprite slot states */
#define SPRITE_FREE    0
#define SPRITE_ACTIVE  1

/* Sprite flags */
#define SPRITE_FLAG_VISIBLE    (1 << 0)
#define SPRITE_FLAG_AFFINE     (1 << 1)
#define SPRITE_FLAG_HFLIP      (1 << 2)
#define SPRITE_FLAG_VFLIP      (1 << 3)
#define SPRITE_FLAG_MOSAIC     (1 << 4)
#define SPRITE_FLAG_SIZE_DBL   (1 << 5)  /* Affine double-size */

/* Sprite info (our bookkeeping, separate from OAM) */
typedef struct {
    u8  state;        /* SPRITE_FREE or SPRITE_ACTIVE */
    u8  flags;        /* Sprite flags */
    u16 tile_id;      /* Starting tile index in VRAM */
    u16 attr2_extra;  /* Palette and priority bits for attr2 */

    /* Position and size in screen pixels */
    s16 x;
    s16 y;
    u8  width;
    u8  height;

    /* Shape and size encoding for OAM */
    u8  shape_size;   /* (shape << 2) | size */

    /* Affine parameters (only if SPRITE_FLAG_AFFINE) */
    u8  affine_index; /* Affine matrix index (0-31) */
} Sprite;

/* -------------------------------------------------------
 *  sprite_init — Initialize sprite manager
 *  Marks all 128 sprite slots as free.
 *  Clears OAM.
 * ------------------------------------------------------- */
void sprite_init(void);

/* -------------------------------------------------------
 *  sprite_alloc — Allocate a sprite slot
 *
 *  width, height: 8, 16, 32, or 64 pixels
 *  tile_id: Starting tile index in OBJ VRAM
 *  palette: Palette bank (0-15)
 *  priority: BG priority (0-3)
 *
 *  Returns sprite slot index (0-127), or -1 if none free.
 * ------------------------------------------------------- */
int sprite_alloc(u8 width, u8 height, u16 tile_id, u8 palette, u8 priority);

/* -------------------------------------------------------
 *  sprite_free — Release a sprite slot back to pool
 *  Hides the sprite immediately.
 * ------------------------------------------------------- */
void sprite_free(int slot);

/* -------------------------------------------------------
 *  sprite_set_pos — Set sprite screen position
 *  x can be -32 to 271 (wraps around)
 *  y can be 0 to 255 (y >= 160 hides sprite)
 * ------------------------------------------------------- */
void sprite_set_pos(int slot, s16 x, s16 y);

/* -------------------------------------------------------
 *  sprite_set_visible — Show/hide sprite
 * ------------------------------------------------------- */
void sprite_set_visible(int slot, bool visible);

/* -------------------------------------------------------
 *  sprite_set_tile — Change sprite's tile index
 *  Used for animation (frame changes).
 * ------------------------------------------------------- */
void sprite_set_tile(int slot, u16 tile_id);

/* -------------------------------------------------------
 *  sprite_set_flip — Set horizontal/vertical flip
 * ------------------------------------------------------- */
void sprite_set_flip(int slot, bool hflip, bool vflip);

/* -------------------------------------------------------
 *  sprite_set_affine — Set affine transform parameters
 *  Sets rotation and scale.
 *  angle: 0-255 (256-degree circle)
 *  sx, sy: scale in 8.8 fixed (256 = 1.0)
 * ------------------------------------------------------- */
void sprite_set_affine(int slot, u8 angle, fixed sx, fixed sy);

/* -------------------------------------------------------
 *  sprite_set_priority — Change sprite's BG priority
 * ------------------------------------------------------- */
void sprite_set_priority(int slot, u8 priority);

/* -------------------------------------------------------
 *  sprite_update_all — Write all active sprites to OAM
 *  Call once per frame, AFTER all sprite modifications.
 * ------------------------------------------------------- */
void sprite_update_all(void);

/* -------------------------------------------------------
 *  sprite_clear_all — Hide all sprites immediately
 * ------------------------------------------------------- */
void sprite_clear_all(void);

/* -------------------------------------------------------
 *  sprite_get — Get pointer to sprite info
 *  Returns NULL if slot is invalid.
 * ------------------------------------------------------- */
Sprite *sprite_get(int slot);

#endif /* SPRITE_MANAGER_H */