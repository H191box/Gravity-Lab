#ifndef SYSTEM_H
#define SYSTEM_H

#include "gba_types.h"

/* System initialization */
void sys_init(void);

/* VBlank handling */
void sys_wait_vblank(void);

/* Frame counter */
u32 sys_get_frame_count(void);

/* Set default palette for testing */
void sys_set_default_palette(void);

/* Register VBlank interrupt handler */
void sys_install_vblank_handler(void);

/* VBlank flag is declared in gba_types.h */

#endif /* SYSTEM_H */
