#ifndef LEVEL_SELECT_H
#define LEVEL_SELECT_H

#include "gba_types.h"

/* Level select cursor */
typedef struct {
    u8 cursor_x;       /* 0-3 column */
    u8 cursor_y;       /* 0-4 row */
    u8 scroll_offset;  /* Scroll offset for levels beyond screen */
} LevelSelect;

/* -------------------------------------------------------
 *  level_select_init — Set up level select screen
 * ------------------------------------------------------- */
void level_select_init(void);

/* -------------------------------------------------------
 *  level_select_update — Handle input for level selection
 *  Returns: selected level ID, or 0xFF if none (still browsing)
 *  Returns 0xFE if player pressed B to go back.
 * ------------------------------------------------------- */
u8 level_select_update(void);

/* -------------------------------------------------------
 *  level_select_render — Draw the level select grid
 * ------------------------------------------------------- */
void level_select_render(void);

#endif /* LEVEL_SELECT_H */