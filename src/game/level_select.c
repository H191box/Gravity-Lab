#include "level_select.h"
#include "input.h"
#include "text_renderer.h"
#include "save.h"
#include "timer_display.h"
#include "medal_system.h"
#include "medal_times.h"
#include "bg_manager.h"
#include "levels.h"
#include "system.h"

/* Level select state */
static LevelSelect lsel;

/* Cursor flash timer */
static u8 cursor_blink = 0;

/* How many columns and rows fit on screen */
#define LS_COLS 4
#define LS_ROWS 5
#define LS_CELL_W 5   /* Tiles per cell width */
#define LS_CELL_H 3   /* Tiles per cell height */
#define LS_OFFSET_X 1  /* Left margin in tiles */
#define LS_OFFSET_Y 3  /* Top margin in tiles (leave room for title) */

/* -------------------------------------------------------
 *  level_select_init — Reset cursor, clear screen
 * ------------------------------------------------------- */
void level_select_init(void) {
    lsel.cursor_x = 0;
    lsel.cursor_y = 0;
    lsel.scroll_offset = 0;
    cursor_blink = 0;

    /* Set up BG for level select (use BG3 for text) */
    bg_disable(BG_LAYER_FLOOR);
    bg_disable(BG_LAYER_WALLS);
    bg_disable(BG_LAYER_DECO);
    bg_enable(BG_LAYER_UI);

    bg_set_priority(BG_LAYER_UI, 0);

    /* Clear text screen */
    text_clear();

    /* Fill background with dark color */
    BG_PALETTE[0] = 0x0000;

    /* Draw title */
    text_draw(7, 0, "GRAVITY LAB", TEXT_COLOR_CYAN);
    text_draw(5, 1, "SELECT CIRCUIT", TEXT_COLOR_WHITE);
}

/* -------------------------------------------------------
 *  level_select_update — Input handling
 * ------------------------------------------------------- */
u8 level_select_update(void) {
    cursor_blink++;

    /* D-pad navigation */
    if (key_just_pressed(KEY_RIGHT)) {
        if (lsel.cursor_x < LS_COLS - 1) {
            lsel.cursor_x++;
        } else if (lsel.cursor_y * LS_COLS + lsel.cursor_x + 1 < level_get_count()) {
            /* Wrap to next row if more levels */
            if (lsel.cursor_y < LS_ROWS - 1) {
                lsel.cursor_x = 0;
                lsel.cursor_y++;
            }
        }
    }
    if (key_just_pressed(KEY_LEFT)) {
        if (lsel.cursor_x > 0) {
            lsel.cursor_x--;
        } else if (lsel.cursor_y > 0) {
            lsel.cursor_x = LS_COLS - 1;
            lsel.cursor_y--;
        }
    }
    if (key_just_pressed(KEY_DOWN)) {
        if (lsel.cursor_y < LS_ROWS - 1) {
            u8 next_row = lsel.cursor_y + 1;
            u8 next_level = next_row * LS_COLS + lsel.cursor_x;
            if (next_level < level_get_count()) {
                lsel.cursor_y = next_row;
            }
        }
    }
    if (key_just_pressed(KEY_UP)) {
        if (lsel.cursor_y > 0) {
            lsel.cursor_y--;
        }
    }

    /* A = Select level */
    if (key_just_pressed(KEY_A) || key_just_pressed(KEY_START)) {
        u8 selected = lsel.cursor_y * LS_COLS + lsel.cursor_x;
        if (selected < level_get_count() && save_is_level_unlocked(selected)) {
            return selected;
        }
    }

    /* B = Back to title */
    if (key_just_pressed(KEY_B)) {
        return 0xFE;
    }

    return 0xFF;  /* Still browsing */
}

/* -------------------------------------------------------
 *  level_select_render — Draw level grid with medals
 * ------------------------------------------------------- */
void level_select_render(void) {
    text_clear();

    /* Title */
    text_draw(7, 0, "GRAVITY LAB", TEXT_COLOR_CYAN);
    text_draw(5, 1, "SELECT CIRCUIT", TEXT_COLOR_WHITE);

    /* Draw level grid */
    u8 row, col;
    u8 level_idx = 0;
    u8 total = level_get_count();

    for (row = 0; row < LS_ROWS; row++) {
        for (col = 0; col < LS_COLS; col++) {
            if (level_idx >= total) break;

            u8 tx = LS_OFFSET_X + col * LS_CELL_W;
            u8 ty = LS_OFFSET_Y + row * LS_CELL_H;

            bool unlocked = save_is_level_unlocked(level_idx);
            bool is_cursor = (col == lsel.cursor_x && row == lsel.cursor_y);

            /* Draw level number */
            if (unlocked) {
                u8 color = TEXT_COLOR_WHITE;

                /* Cursor highlight */
                if (is_cursor) {
                    color = (cursor_blink & 0x10) ? TEXT_COLOR_YELLOW : TEXT_COLOR_WHITE;
                }

                /* Level number */
                text_draw_char(tx + 1, ty, '0' + (level_idx / 10) % 10, color);
                text_draw_char(tx + 2, ty, '0' + level_idx % 10, color);

                /* Level name (first 10 chars) */
                const char *name = level_get_name(level_idx);
                u8 ci = 0;
                while (name[ci] && ci < 10) {
                    text_draw_char(tx + ci, ty + 1, name[ci], TEXT_COLOR_GRAY);
                    ci++;
                }

                /* Medal display */
                u32 best = gt_load_best(level_idx);
                if (best > 0) {
                    u8 medal = medal_calculate(best, level_idx);
                    const char *medal_name = medal_get_name(medal);
                    u8 medal_color = medal_get_color(medal);
                    u8 mi = 0;
                    while (medal_name[mi]) {
                        text_draw_char(tx + mi, ty + 2, medal_name[mi], medal_color);
                        mi++;
                    }
                } else {
                    text_draw(tx + 1, ty + 2, "---", TEXT_COLOR_DARK);
                }

                /* Best time display */
                if (best > 0) {
                    text_draw_time(tx, ty + 3, best, TEXT_COLOR_GRAY);
                }
            } else {
                /* Locked level */
                text_draw_char(tx + 1, ty, '?', TEXT_COLOR_DARK);
                text_draw_char(tx + 2, ty, '?', TEXT_COLOR_DARK);
                text_draw(tx, ty + 1, "LOCKED", TEXT_COLOR_DARK);
            }

            level_idx++;
        }
    }

    /* Instructions at bottom */
    text_draw(1, 18, "A:SELECT  B:BACK", TEXT_COLOR_GRAY);
}