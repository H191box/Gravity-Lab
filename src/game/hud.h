#ifndef HUD_H
#define HUD_H

#include "gba_types.h"

/* -------------------------------------------------------
 *  hud_init — Set up HUD layer and styles
 * ------------------------------------------------------- */
void hud_init(void);

/* -------------------------------------------------------
 *  hud_update — Update HUD data each frame
 *  level_id, checkpoint_cur, checkpoint_total
 * ------------------------------------------------------- */
void hud_update(u8 level_id, u8 checkpoint_cur, u8 checkpoint_total);

/* -------------------------------------------------------
 *  hud_render — Draw HUD elements
 *  Timer, level name, checkpoint counter
 * ------------------------------------------------------- */
void hud_render(void);

/* -------------------------------------------------------
 *  hud_show_message — Display a temporary message
 *  msg: string to show
 *  duration: frames to display
 * ------------------------------------------------------- */
void hud_show_message(const char *msg, u8 duration);

#endif /* HUD_H */