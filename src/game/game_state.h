#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "gba_types.h"

/* Game states */
#define STATE_TITLE         0
#define STATE_LEVEL_SELECT  1
#define STATE_PLAYING       2
#define STATE_EXPLODING     3
#define STATE_COMPLETE      4
#define STATE_PAUSED        5

/* -------------------------------------------------------
 *  game_init — One-time game initialization
 *  Sets up all subsystems.
 * ------------------------------------------------------- */
void game_init(void);

/* -------------------------------------------------------
 *  game_update — Main update tick
 *  Dispatches to the current state's update function.
 * ------------------------------------------------------- */
void game_update(void);

/* -------------------------------------------------------
 *  game_render — Main render tick
 *  Dispatches to the current state's render function.
 * ------------------------------------------------------- */
void game_render(void);

/* -------------------------------------------------------
 *  game_change_state — Transition to a new state
 *  Handles cleanup of old state and init of new state.
 * ------------------------------------------------------- */
void game_change_state(u8 new_state);

/* -------------------------------------------------------
 *  game_get_state — Get current game state
 * ------------------------------------------------------- */
u8 game_get_state(void);

/* -------------------------------------------------------
 *  game_start_level — Begin playing a specific level
 * ------------------------------------------------------- */
void game_start_level(u8 level_id);

#endif /* GAME_STATE_H */