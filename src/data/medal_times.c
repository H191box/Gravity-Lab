#include "medal_times.h"

/* ============================================================
 *  Medal Time Thresholds
 *  Times in centiseconds (1/100 second).
 *  Format: MM:SS.CC
 *
 *  Level 1 "First Launch":    Gold 15.00  Silver 25.00  Bronze 40.00
 *  Level 2 "Narrow Pass":     Gold 20.00  Silver 35.00  Bronze 55.00
 *  Level 3 "Boost Zone":      Gold 18.00  Silver 30.00  Bronze 45.00
 *  Level 4 "Spinning Death":  Gold 25.00  Silver 40.00  Bronze 60.00
 *  Level 5 "The Gauntlet":    Gold 30.00  Silver 50.00  Bronze 75.00
 *
 *  For levels 6-19 (designed but not yet implemented):
 *  Progressive difficulty scaling.
 * ============================================================ */

#define NUM_MEDAL_LEVELS 20

static const MedalTimes medal_thresholds[NUM_MEDAL_LEVELS] = {
    /* Level 1: "First Launch" */
    { 1500, 2500, 4000 },
    /* Level 2: "Narrow Pass" */
    { 2000, 3500, 5500 },
    /* Level 3: "Boost Zone" */
    { 1800, 3000, 4500 },
    /* Level 4: "Spinning Death" */
    { 2500, 4000, 6000 },
    /* Level 5: "The Gauntlet" */
    { 3000, 5000, 7500 },
    /* Level 6: "U-Turn" */
    { 2200, 3800, 5800 },
    /* Level 7: "Speed Strip" */
    { 1600, 2800, 4200 },
    /* Level 8: "Maze Runner" */
    { 3500, 5500, 8000 },
    /* Level 9: "Blade Alley" */
    { 2800, 4500, 6500 },
    /* Level 10: "The Funnel" */
    { 3200, 5200, 7800 },
    /* Level 11: "Double Cross" */
    { 3800, 6000, 9000 },
    /* Level 12: "Warp Speed" */
    { 2000, 3500, 5000 },
    /* Level 13: "Pinball" */
    { 4000, 6500, 9500 },
    /* Level 14: "The Serpentine" */
    { 4500, 7000, 10000 },
    /* Level 15: "Obstacle Course" */
    { 5000, 8000, 12000 },
    /* Level 16: "Gravity Well" */
    { 4200, 6800, 10000 },
    /* Level 17: "The Labyrinth" */
    { 5500, 8500, 13000 },
    /* Level 18: "Final Approach" */
    { 6000, 9000, 14000 },
    /* Level 19: "Champion's Circuit" */
    { 6500, 10000, 15000 },
    /* Level 20: "Gravity Lab Ultimate" */
    { 7000, 11000, 16000 },
};

void medal_times_get(u8 level_id, MedalTimes *out) {
    if (!out) return;

    if (level_id < NUM_MEDAL_LEVELS) {
        *out = medal_thresholds[level_id];
    } else {
        /* Default thresholds for unknown levels */
        out->gold   = 99999;
        out->silver = 99999;
        out->bronze = 99999;
    }
}
