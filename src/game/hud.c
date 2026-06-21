#include "hud.h"
#include "text_renderer.h"
#include "timer_display.h"
#include "levels.h"
#include "bg_manager.h"
#include "system.h"

/* HUD state */
static u8 hud_level_id = 0;
static u8 hud_cp_cur = 0;
static u8 hud_cp_total = 0;
static u8 hud_initialized = 0;

/* Temporary message */
static const char *hud_msg = (const char *)NULL;
static u8 hud_msg_timer = 0;

/* -------------------------------------------------------
 *  hud_init — Set up the HUD text layer
 *  Uses BG3 (text layer) with high priority.
 * ------------------------------------------------------- */
void hud_init(void) {
    if (hud_initialized) return;
    hud_initialized = 1;

    /* Text layer is already initialized by text_init().
     * Just set up priority so it renders above game BGs. */
    bg_set_priority(BG_LAYER_UI, 3);
    bg_enable(BG_LAYER_UI);
}

/* -------------------------------------------------------
 *  hud_update — Store current HUD data
 * ------------------------------------------------------- */
void hud_update(u8 level_id, u8 checkpoint_cur, u8 checkpoint_total) {
    hud_level_id = level_id;
    hud_cp_cur = checkpoint_cur;
    hud_cp_total = checkpoint_total;

    /* Update message timer */
    if (hud_msg_timer > 0) {
        hud_msg_timer--;
        if (hud_msg_timer == 0) {
            hud_msg = (const char *)NULL;
        }
    }
}

/* -------------------------------------------------------
 *  hud_render — Draw HUD text
 *
 *  Layout:
 *  Top-left:    Level name
 *  Top-center:  Timer MM:SS.CC
 *  Top-right:   Checkpoint X/Y
 *  Bottom:      Temporary message (if any)
 * ------------------------------------------------------- */
void hud_render(void) {
    /* Level name */
    const char *name = level_get_name(hud_level_id);
    text_draw(1, 0, name, TEXT_COLOR_WHITE);

    /* Timer (centered at top) */
    timer_render(10, 0, TEXT_COLOR_YELLOW);

    /* Checkpoint counter */
    text_draw(22, 0, "CP:", TEXT_COLOR_GREEN);
    text_draw_int(25, 0, (s32)hud_cp_cur, 1, TEXT_COLOR_GREEN);
    text_draw_char(26, 0, '/', TEXT_COLOR_GREEN);
    text_draw_int(27, 0, (s32)hud_cp_total, 1, TEXT_COLOR_GREEN);

    /* Speed indicator (bottom-left) */
    /* (Simple: just show "A:THRUST B:BRAKE") */
    text_draw(0, 19, "L/R:TURN A:GAS B:BRAKE", TEXT_COLOR_DARK);

    /* Temporary message */
    if (hud_msg && hud_msg_timer > 0) {
        u8 color = TEXT_COLOR_YELLOW;
        if (hud_msg_timer < 30 && (hud_msg_timer & 2) == 0) {
            color = TEXT_COLOR_WHITE;
        }
        u8 msg_x = (u8)(15 - 0);  /* Roughly centered */
        text_draw(msg_x, 9, hud_msg, color);
    }
}

/* -------------------------------------------------------
 *  hud_show_message — Display temporary text
 * ------------------------------------------------------- */
void hud_show_message(const char *msg, u8 duration) {
    hud_msg = msg;
    hud_msg_timer = duration;
}