#ifndef FONT_H
#define FONT_H

#include "gba_types.h"

/* Number of printable characters (space through ~) */
#define FONT_CHARS      95
#define FONT_CHAR_START 0x20  /* ASCII space */
#define FONT_CHAR_END   0x7E  /* ASCII ~ */

/* Font is 8x8 pixels, stored as 1bpp (8 bytes per glyph) */
/* Row format: MSB = leftmost pixel */

/* Font glyph data: 95 characters × 8 bytes each = 760 bytes */
extern const u8 font_data[FONT_CHARS][8];

#endif /* FONT_H */
