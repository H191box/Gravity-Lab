#ifndef MEDAL_TIMES_H
#define MEDAL_TIMES_H

#include "gba_types.h"

/* Medal time thresholds per level */
typedef struct {
    u32 gold;    /* Gold medal time in centiseconds */
    u32 silver;  /* Silver medal time in centiseconds */
    u32 bronze;  /* Bronze medal time in centiseconds */
} MedalTimes;

/* -------------------------------------------------------
 *  medal_times_get — Get medal thresholds for a level
 *  level_id: level index (0-based)
 *  out: pointer to MedalTimes to fill
 * ------------------------------------------------------- */
void medal_times_get(u8 level_id, MedalTimes *out);

#endif /* MEDAL_TIMES_H */
