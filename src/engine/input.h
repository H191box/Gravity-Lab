#ifndef INPUT_H
#define INPUT_H

#include "gba_types.h"

/* Key constants */
#define KEY_A          (1 << 0)
#define KEY_B          (1 << 1)
#define KEY_SELECT     (1 << 2)
#define KEY_START      (1 << 3)
#define KEY_RIGHT      (1 << 4)
#define KEY_LEFT       (1 << 5)
#define KEY_UP         (1 << 6)
#define KEY_DOWN       (1 << 7)
#define KEY_R          (1 << 8)
#define KEY_L          (1 << 9)

/* Input state tracking */
typedef struct {
    u16 current;      /* Keys held this frame */
    u16 previous;     /* Keys held last frame */
    u16 pressed;      /* Keys currently pressed (held) */
    u16 just_pressed; /* Keys pressed this frame but not last */
    u16 released;     /* Keys released this frame */
} InputState;

/* Initialize input system */
void input_init(void);

/* Update input state — call once per frame, BEFORE game logic */
void input_update(void);

/* Query functions */
bool key_pressed(u16 key);
bool key_just_pressed(u16 key);
bool key_released(u16 key);

/* Get raw key state */
u16 input_get_raw(void);

#endif /* INPUT_H */