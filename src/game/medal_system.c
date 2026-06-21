#include "medal_system.h"
#include "sprite_manager.h"
#include "medal_times.h"

/* Medal sprite tile indices */
/* Tiles 24-27: Gold medal (16x16 = 4 tiles) */
/* Tiles 28-31: Silver medal (16x16 = 4 tiles) */
/* Tiles 32-35: Bronze medal (16x16 = 4 tiles) */
#define MEDAL_GOLD_TILE   24
#define MEDAL_SILVER_TILE 28
#define MEDAL_BRONZE_TILE 32

/* -------------------------------------------------------
 *  medal_calculate — Compare time against thresholds
 * ------------------------------------------------------- */
u8 medal_calculate(u32 time_cs, u8 level_id) {
    MedalTimes times;
    medal_times_get(level_id, &times);

    if (time_cs <= times.gold) {
        return MEDAL_GOLD;
    } else if (time_cs <= times.silver) {
        return MEDAL_SILVER;
    } else if (time_cs <= times.bronze) {
        return MEDAL_BRONZE;
    } else {
        return MEDAL_NONE;
    }
}

/* -------------------------------------------------------
 *  medal_render — Draw medal sprite
 *  We allocate a sprite for the medal display.
 *  The caller should free it when done.
 * ------------------------------------------------------- */
static int medal_sprite_slot = -1;

void medal_render(u8 type, s16 screen_x, s16 screen_y) {
    /* Free previous medal sprite */
    if (medal_sprite_slot >= 0) {
        sprite_free(medal_sprite_slot);
        medal_sprite_slot = -1;
    }

    if (type == MEDAL_NONE) return;

    u16 tile = 0;
    switch (type) {
        case MEDAL_GOLD:   tile = MEDAL_GOLD_TILE; break;
        case MEDAL_SILVER: tile = MEDAL_SILVER_TILE; break;
        case MEDAL_BRONZE: tile = MEDAL_BRONZE_TILE; break;
        default: return;
    }

    medal_sprite_slot = sprite_alloc(16, 16, tile, 0, 0);
    if (medal_sprite_slot >= 0) {
        sprite_set_pos(medal_sprite_slot, screen_x - 8, screen_y - 8);
        sprite_set_visible(medal_sprite_slot, TRUE);
    }
}

/* -------------------------------------------------------
 *  medal_get_name — Get display name
 * ------------------------------------------------------- */
const char *medal_get_name(u8 type) {
    switch (type) {
        case MEDAL_GOLD:   return "GOLD";
        case MEDAL_SILVER: return "SILVER";
        case MEDAL_BRONZE: return "BRONZE";
        default:           return "NONE";
    }
}

/* -------------------------------------------------------
 *  medal_get_color — Get text color for medal
 * ------------------------------------------------------- */
u8 medal_get_color(u8 type) {
    switch (type) {
        case MEDAL_GOLD:   return 7;   /* Gold sub-palette */
        case MEDAL_SILVER: return 6;   /* Silver sub-palette */
        case MEDAL_BRONZE: return 6;   /* Use gray for bronze text */
        default:           return 1;   /* White */
    }
}

/* -------------------------------------------------------
 *  medal_free_sprite — Clean up medal sprite
 * ------------------------------------------------------- */
void medal_free_sprite(void) {
    if (medal_sprite_slot >= 0) {
        sprite_free(medal_sprite_slot);
        medal_sprite_slot = -1;
    }
}