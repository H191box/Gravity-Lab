#include "save.h"
#include "gba_header.h"
#include "dma.h"

/* SRAM write delay: must wait between writes */
static void sram_wait(void) {
    volatile u16 delay = 0;
    delay++;
    delay++;
}

/* -------------------------------------------------------
 *  save_compute_checksum — Simple 16-bit checksum
 * ------------------------------------------------------- */
static u16 save_compute_checksum(const SaveData *data) {
    const u16 *ptr = (const u16 *)data;
    u16 sum = 0;
    u32 i;

    /* Sum all words except the checksum field (last word) */
    for (i = 0; i < (sizeof(SaveData) - 2) / 2; i++) {
        sum += ptr[i];
    }
    return sum;
}

/* -------------------------------------------------------
 *  save_read — Load save data from SRAM
 *  Reads slowly (byte by byte) because SRAM access is slow.
 * ------------------------------------------------------- */
bool save_read(SaveData *out) {
    if (!out) return FALSE;

    /* Copy from SRAM to a temp buffer */
    u8 *src = (u8 *)SRAM_BASE;
    u8 *dst = (u8 *)out;
    u32 i;

    for (i = 0; i < sizeof(SaveData); i++) {
        dst[i] = src[i];
    }

    /* Validate magic number */
    if (out->magic != SAVE_MAGIC) {
        return FALSE;
    }

    /* Validate checksum */
    u16 expected = save_compute_checksum(out);
    if (out->checksum != expected) {
        return FALSE;
    }

    return TRUE;
}

/* -------------------------------------------------------
 *  save_write — Write save data to SRAM
 *  SRAM is accessed byte by byte with delays.
 * ------------------------------------------------------- */
void save_write(const SaveData *data) {
    if (!data) return;

    /* Create a writable copy to set checksum */
    SaveData temp;
    const u8 *src = (const u8 *)data;
    u8 *dst = (u8 *)&temp;
    u32 i;

    /* Copy data */
    for (i = 0; i < sizeof(SaveData); i++) {
        dst[i] = src[i];
    }

    /* Set magic and compute checksum */
    temp.magic = SAVE_MAGIC;
    temp.checksum = save_compute_checksum(&temp);

    /* Write to SRAM byte by byte */
    u8 *sram = (u8 *)SRAM_BASE;
    for (i = 0; i < sizeof(SaveData); i++) {
        sram[i] = dst[i];
        sram_wait();
    }
}

/* -------------------------------------------------------
 *  save_reset — Clear SRAM and write default save
 * ------------------------------------------------------- */
void save_reset(void) {
    SaveData default_save;
    u32 i;

    default_save.magic = SAVE_MAGIC;
    default_save.unlocked = 0x00000001;  /* Level 0 unlocked */
    for (i = 0; i < MAX_LEVELS; i++) {
        default_save.best_times[i] = 0;
    }
    default_save.checksum = 0;
    default_save.checksum = save_compute_checksum(&default_save);

    save_write(&default_save);
}

/* -------------------------------------------------------
 *  save_is_valid — Check SRAM has valid data
 * ------------------------------------------------------- */
bool save_is_valid(void) {
    SaveData temp;
    return save_read(&temp);
}

/* -------------------------------------------------------
 *  save_is_level_unlocked — Check unlock status
 * ------------------------------------------------------- */
bool save_is_level_unlocked(u8 level_id) {
    if (level_id >= MAX_LEVELS) return FALSE;

    SaveData save;
    if (!save_read(&save)) {
        /* No valid save — only level 0 is unlocked */
        return level_id == 0;
    }

    return (save.unlocked & (1u << level_id)) != 0;
}

/* -------------------------------------------------------
 *  save_unlock_level — Set a level as unlocked
 * ------------------------------------------------------- */
void save_unlock_level(u8 level_id) {
    if (level_id >= MAX_LEVELS) return;

    SaveData save;
    if (!save_read(&save)) {
        save_reset();
        save_read(&save);
    }

    save.unlocked |= (1u << level_id);
    save_write(&save);
}