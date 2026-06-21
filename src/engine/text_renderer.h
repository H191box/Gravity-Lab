#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "gba_types.h"

/* Text color indices (in BG palette) */
#define TEXT_COLOR_WHITE    1
#define TEXT_COLOR_RED      6
#define TEXT_COLOR_GREEN    8
#define TEXT_COLOR_YELLOW   9
#define TEXT_COLOR_CYAN     10
#define TEXT_COLOR_GRAY     3
#define TEXT_COLOR_SILVER   6
#define TEXT_COLOR_GOLD     12
#define TEXT_COLOR_BRONZE   14
#define TEXT_COLOR_DARK     2

/* Text layer (BG3 by default) */
#define TEXT_BG_LAYER  BG_LAYER_UI

/* -------------------------------------------------------
 *  text_init — Load font tiles into BG character data
 *  and configure the text background layer.
 *
 *  Font is 8x8, 4bpp, using palette entries 0-1.
 *  We load into CharBase 1 (BG3's character base).
 * ------------------------------------------------------- */
void text_init(void);

/* -------------------------------------------------------
 *  text_clear — Clear the text screen
 *  Fills the tile map with tile 0 (blank).
 * ------------------------------------------------------- */
void text_clear(void);

/* -------------------------------------------------------
 *  text_draw — Draw a string at tile position (x, y)
 *  x, y: tile coordinates (0-29 for x, 0-19 for y)
 *  str: null-terminated string
 *  color: palette index for text color
 *
 *  Returns the number of characters drawn.
 * ------------------------------------------------------- */
int text_draw(u8 x, u8 y, const char *str, u8 color);

/* -------------------------------------------------------
 *  text_draw_char — Draw a single character
 * ------------------------------------------------------- */
void text_draw_char(u8 x, u8 y, char ch, u8 color);

/* -------------------------------------------------------
 *  text_draw_int — Draw an integer at tile position
 *  x, y: tile coordinates
 *  num: number to display
 *  digits: minimum number of digits (padded with spaces)
 *  color: palette color index
 * ------------------------------------------------------- */
int text_draw_int(u8 x, u8 y, s32 num, u8 digits, u8 color);

/* -------------------------------------------------------
 *  text_draw_time — Draw time in MM:SS.CC format
 *  centiseconds: time in 1/100th seconds
 *  color: palette color index
 * ------------------------------------------------------- */
int text_draw_time(u8 x, u8 y, u32 centiseconds, u8 color);

/* -------------------------------------------------------
 *  text_set_layer — Change which BG layer text renders to
 *  Useful if text layer conflicts with game BGs.
 * ------------------------------------------------------- */
void text_set_layer(int layer);

#endif /* TEXT_RENDERER_H */