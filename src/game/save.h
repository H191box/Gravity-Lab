#ifndef SAVE_H
#define SAVE_H

#include "gba_types.h"

#define MAX_LEVELS 20

/* Save data magic number */
#define SAVE_MAGIC  0x47564C42  /* "GVLB" = Gravity LaB */

/* Save data structure — fits in SRAM */
typedef struct {
    u32 magic;                /* Magic number for validation */
    u32 unlocked;             /* Bitfield: bit N set = level N unlocked */
    u32 best_times[MAX_LEVELS]; /* Best time per level in centiseconds */
    u16 checksum;             /* Simple checksum */
} SaveData;

/* -------------------------------------------------------
 *  save_read — Load save data from SRAM
 *  Returns TRUE if save data is valid.
 *  out: pointer to SaveData to fill.
 * ------------------------------------------------------- */
bool save_read(SaveData *out);

/* -------------------------------------------------------
 *  save_write — Write save data to SRAM
 *  Automatically computes checksum.
 * ------------------------------------------------------- */
void save_write(const SaveData *data);

/* -------------------------------------------------------
 *  save_reset — Clear all save data (new game)
 * ------------------------------------------------------- */
void save_reset(void);

/* -------------------------------------------------------
 *  save_is_valid — Check if SRAM has valid save
 * ------------------------------------------------------- */
bool save_is_valid(void);

/* -------------------------------------------------------
 *  save_is_level_unlocked — Check if a level is unlocked
 * ------------------------------------------------------- */
bool save_is_level_unlocked(u8 level_id);

/* -------------------------------------------------------
 *  save_unlock_level — Unlock a specific level
 * ------------------------------------------------------- */
void save_unlock_level(u8 level_id);

#endif /* SAVE_H */