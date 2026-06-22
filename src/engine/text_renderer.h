#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "gba_types.h"

/* Text color indices — must match text_init() sub-palette layout.
 * text_init() sets sub-palette N with colors[N]:
 *   Sub-pal 0: White, Sub-pal 1: Red, Sub-pal 2: Green, ...
 * So TEXT_COLOR_WHITE=0 means sub-palette 0 = white foreground.
 */
#define TEXT_COLOR_WHITE    0
#define TEXT_COLOR_RED      1
#define TEXT_COLOR_GREEN    2
#define TEXT_COLOR_YELLOW   3
#define TEXT_COLOR_CYAN     4
#define TEXT_COLOR_GRAY     5
#define TEXT_COLOR_SILVER   5
#define TEXT_COLOR_GOLD     6
#define TEXT_COLOR_BRONZE   7
#define TEXT_COLOR_DARK     1

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