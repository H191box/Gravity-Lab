#ifndef GRAVITY_LAB_H
#define GRAVITY_LAB_H

/* ============================================================
 *  Gravity Lab — Master Header
 *  Circuit Racing Puzzle Game for GBA
 *
 *  Version: 0.1.0
 *  Target: Game Boy Advance (ARM7TDMI, devkitARM)
 * ============================================================ */

/* Game identification */
#define GAME_NAME       "GRAVITY LAB"
#define GAME_VERSION    "0.1.0"
#define GAME_TITLE      "GRAVITY LAB"

/* Game constants */
#define MAX_LEVELS      20
#define NUM_IMPLEMENTED_LEVELS 5

/* Physics tuning */
#define PHYSICS_FPS     60
#define PHYSICS_DRAG    245
#define PHYSICS_THRUST  180
#define PHYSICS_BRAKE   100
#define PHYSICS_ROTspd  3
#define PHYSICS_MAXspd  (1 << 10)

/* Display constants */
#define HUD_HEIGHT      16
#define MSG_DURATION    120

/* Medal colors for text */
#define MEDAL_TEXT_GOLD   7
#define MEDAL_TEXT_SILVER 6
#define MEDAL_TEXT_BRONZE 6

/* ============================================================
 *  Includes — Engine
 * ============================================================ */
#include "gba_types.h"
#include "system.h"
#include "input.h"
#include "dma.h"
#include "timer.h"
#include "fixed.h"
#include "math_util.h"
#include "bg_manager.h"
#include "sprite_manager.h"
#include "obj_tiles.h"
#include "particle_system.h"
#include "text_renderer.h"

/* ============================================================
 *  Includes — Game Logic
 * ============================================================ */
#include "ship.h"
/* Data includes BEFORE circuit (circuit.h forward-declares LevelData) */
#include "levels.h"
#include "medal_times.h"
#include "circuit.h"
#include "obstacle.h"
#include "checkpoint.h"
#include "explosion.h"
#include "timer_display.h"
#include "medal_system.h"
#include "level_select.h"
#include "hud.h"
#include "game_state.h"
#include "save.h"

/* ============================================================
 *  Includes — Assets
 * ============================================================ */
#include "tiles.h"
#include "palettes.h"
#include "sprites.h"
#include "font.h"

#endif /* GRAVITY_LAB_H */
