#include "input.h"

/* Global input state */
static InputState input_state = {0};

/* -------------------------------------------------------
 *  input_init — Clear input state
 *  Called once at startup before the main loop.
 * ------------------------------------------------------- */
void input_init(void) {
    input_state.current = REG_KEYPAD;
    input_state.previous = input_state.current;
    input_state.pressed = input_state.current ^ 0x03FF;
    input_state.just_pressed = 0;
    input_state.released = 0;
}

/* -------------------------------------------------------
 *  input_update — Poll keypad and compute edge states
 *  MUST be called once per frame BEFORE any key queries.
 *
 *  REG_KEYPAD is active-low:
 *    bit=0 means key is held, bit=1 means key is released.
 *  We invert so our internal state is active-high.
 * ------------------------------------------------------- */
void input_update(void) {
    /* Save previous frame's state */
    input_state.previous = input_state.current;

    /* Read current hardware state and invert to active-high */
    input_state.current = REG_KEYPAD ^ 0x03FF;

    /* Held keys */
    input_state.pressed = input_state.current;

    /* Keys pressed this frame (current && !previous) */
    input_state.just_pressed = input_state.current & ~input_state.previous;

    /* Keys released this frame (!current && previous) */
    input_state.released = ~input_state.current & input_state.previous;
}

/* -------------------------------------------------------
 *  key_pressed — Is this key currently held down?
 * ------------------------------------------------------- */
bool key_pressed(u16 key) {
    return (input_state.pressed & key) != 0;
}

/* -------------------------------------------------------
 *  key_just_pressed — Was this key just pressed this frame?
 *  Only true for one frame on the press edge.
 * ------------------------------------------------------- */
bool key_just_pressed(u16 key) {
    return (input_state.just_pressed & key) != 0;
}

/* -------------------------------------------------------
 *  key_released — Was this key just released this frame?
 * ------------------------------------------------------- */
bool key_released(u16 key) {
    return (input_state.released & key) != 0;
}

/* -------------------------------------------------------
 *  input_get_raw — Get the inverted raw keypad state
 * ------------------------------------------------------- */
u16 input_get_raw(void) {
    return input_state.current;
}