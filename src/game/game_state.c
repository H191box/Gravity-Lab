#include "game_state.h"
#include "system.h"
#include "input.h"
#include "sprite_manager.h"
#include "particle_system.h"
#include "text_renderer.h"
#include "bg_manager.h"
#include "timer.h"
#include "math_util.h"

#include "ship.h"
#include "circuit.h"
#include "obstacle.h"
#include "checkpoint.h"
#include "explosion.h"
#include "timer_display.h"
#include "medal_system.h"
#include "level_select.h"
#include "hud.h"
#include "save.h"

#include "tiles.h"
#include "palettes.h"
#include "sprites.h"

/* Current game state */
static u8 game_state = STATE_TITLE;
static u8 pending_level = 0;

/* Title screen variables */
static u8 title_timer = 0;

/* Level complete variables */
static u32 completion_time = 0;
static u8  completion_medal = MEDAL_NONE;
static u8  complete_timer = 0;

/* Pause state */
static u8 pause_blink = 0;

/* -------------------------------------------------------
 *  game_init — Full initialization
 * ------------------------------------------------------- */
void game_init(void) {
    /* System init */
    sys_init();
    sys_set_default_palette();
    input_init();

    /* Load game palettes */
    palettes_load();

    /* Load tile graphics into VRAM */
    tiles_load();

    /* Load sprite graphics into OBJ VRAM */
    sprites_load();

    /* Initialize subsystems */
    sprite_init();
    particle_init();
    text_init();
    bg_init();

    /* Initialize save system */
    if (!save_is_valid()) {
        save_reset();
    }

    /* Game timer */
    timer_display_init();

    /* Start at title screen */
    game_change_state(STATE_TITLE);
}

/* -------------------------------------------------------
 *  game_change_state — State machine transition
 * ------------------------------------------------------- */
void game_change_state(u8 new_state) {
    /* Cleanup old state */
    switch (game_state) {
        case STATE_PLAYING:
        case STATE_EXPLODING:
            /* Stop timer */
            gt_stop();
            /* Clear game objects */
            obstacle_clear();
            particle_clear_all();
            break;
    }

    game_state = new_state;

    /* Initialize new state */
    switch (new_state) {
        case STATE_TITLE:
            title_timer = 0;
            sprite_clear_all();
            particle_clear_all();
            bg_disable(BG_LAYER_FLOOR);
            bg_disable(BG_LAYER_WALLS);
            bg_disable(BG_LAYER_DECO);
            bg_enable(BG_LAYER_UI);
            bg_set_priority(BG_LAYER_UI, 0);
            text_clear();
            break;

        case STATE_LEVEL_SELECT:
            level_select_init();
            sprite_clear_all();
            particle_clear_all();
            break;

        case STATE_PLAYING:
            /* Level setup is done in game_start_level */
            break;

        case STATE_COMPLETE:
            complete_timer = 0;
            break;

        case STATE_PAUSED:
            pause_blink = 0;
            gt_stop();
            break;
    }
}

/* -------------------------------------------------------
 *  game_start_level — Initialize and start a level
 * ------------------------------------------------------- */
void game_start_level(u8 level_id) {
    /* Clear everything */
    sprite_clear_all();
    particle_clear_all();

    /* Get start position */
    s16 sx, sy;
    u8 sa;
    level_get_start_pos(level_id, &sx, &sy, &sa);

    /* Initialize ship at start position */
    ship_init(sx, sy, sa);

    /* Load circuit tiles into BG */
    circuit_load(level_id);

    /* Initialize obstacles */
    obstacle_init(level_id);

    /* Initialize checkpoints */
    checkpoint_init(level_id);

    /* Initialize HUD */
    hud_init();

    /* Reset and start timer */
    gt_reset();
    gt_start();

    /* Change to playing state */
    game_state = STATE_PLAYING;
    pending_level = level_id;
}

/* -------------------------------------------------------
 *  game_update — Main update dispatcher
 * ------------------------------------------------------- */
void game_update(void) {
    switch (game_state) {
        case STATE_TITLE:
            title_timer++;
            if (key_just_pressed(KEY_START) || key_just_pressed(KEY_A)) {
                game_change_state(STATE_LEVEL_SELECT);
            }
            break;

        case STATE_LEVEL_SELECT: {
            u8 result = level_select_update();
            if (result < 0xFE) {
                game_start_level(result);
            } else if (result == 0xFE) {
                game_change_state(STATE_TITLE);
            }
            break;
        }

        case STATE_PLAYING: {
            /* Update ship physics */
            ship_update();

            Ship *s = ship_get();
            s16 ship_wx = fixed_to_int(s->x);
            s16 ship_wy = fixed_to_int(s->y);

            /* Check wall/tile collisions */
            if (!ship_is_invincible()) {
                /* Check ship's 4 corners for wall collision */
                u8 tile = circuit_get_tile(ship_wx, ship_wy);
                if (circuit_is_solid(tile)) {
                    /* Collision with wall! */
                    s16 screen_x = ship_wx - camera_x;
                    s16 screen_y = ship_wy - camera_y;
                    explosion_start(screen_x, screen_y);
                    ship_explode();
                    game_state = STATE_EXPLODING;
                    break;
                }

                /* Check obstacle collisions */
                if (obstacle_check_collision(ship_wx, ship_wy, SHIP_COLLISION_RADIUS)) {
                    s16 screen_x = ship_wx - camera_x;
                    s16 screen_y = ship_wy - camera_y;
                    explosion_start(screen_x, screen_y);
                    ship_explode();
                    game_state = STATE_EXPLODING;
                    break;
                }
            }

            /* Check boost pad collision */
            u8 floor_tile = circuit_get_tile(ship_wx, ship_wy);
            if (floor_tile == TILE_BOOST_PAD) {
                ship_apply_boost();
            }

            /* Check checkpoints */
            checkpoint_update(ship_wx, ship_wy);

            /* Check if all checkpoints passed and at finish line */
            if (checkpoint_all_passed()) {
                u8 finish_tile = circuit_get_tile(ship_wx, ship_wy);
                if (finish_tile == TILE_FINISH) {
                    /* Level complete! */
                    completion_time = gt_get_time();
                    gt_stop();
                    completion_medal = medal_calculate(completion_time, pending_level);
                    gt_save_best(pending_level, completion_time);
                    game_change_state(STATE_COMPLETE);
                    break;
                }
            }

            /* Update obstacles */
            obstacle_update_all();

            /* Update camera */
            circuit_update();

            /* Update particles */
            particle_update_all();

            /* Update screen shake */
            shake_update();

            /* Update HUD */
            hud_update(pending_level, checkpoint_get_next(), checkpoint_get_total());

            /* Pause */
            if (key_just_pressed(KEY_START)) {
                game_change_state(STATE_PAUSED);
            }
            break;
        }

        case STATE_EXPLODING: {
            /* Update explosion animation */
            bool done = explosion_update();

            /* Still update particles and shake during explosion */
            particle_update_all();
            shake_update();

            if (done) {
                /* Respawn ship at last checkpoint */
                s16 rx, ry;
                u8 ra;
                checkpoint_get_respawn_pos(&rx, &ry, &ra);
                ship_respawn(rx, ry, ra);
                particle_clear_all();
                game_state = STATE_PLAYING;
            }
            break;
        }

        case STATE_COMPLETE:
            complete_timer++;
            particle_update_all();

            /* Spawn celebration particles */
            if (complete_timer < 60 && complete_timer % 4 == 0) {
                s16 px = (s16)(random_next() % SCREEN_W);
                s16 py = (s16)(random_next() % (SCREEN_H / 2));
                particle_spawn_burst(PARTICLE_MEDAL_SPARKLE, px, py, 4, 1, 3, 20);
            }

            if (complete_timer > 180 || key_just_pressed(KEY_A) || key_just_pressed(KEY_START)) {
                game_change_state(STATE_LEVEL_SELECT);
            }
            break;

        case STATE_PAUSED:
            pause_blink++;
            if (key_just_pressed(KEY_START) || key_just_pressed(KEY_B)) {
                game_state = STATE_PLAYING;
                gt_start();
            }
            if (key_just_pressed(KEY_SELECT)) {
                /* Quit to level select */
                game_change_state(STATE_LEVEL_SELECT);
            }
            break;
    }
}

/* -------------------------------------------------------
 *  game_render — Main render dispatcher
 * ------------------------------------------------------- */
void game_render(void) {
    switch (game_state) {
        case STATE_TITLE:
            /* Draw title screen */
            text_clear();

            /* Title art */
            text_draw(3, 4, "GRAVITY LAB", TEXT_COLOR_CYAN);
            text_draw(5, 5, "========", TEXT_COLOR_CYAN);

            /* Subtitle */
            text_draw(2, 7, "CIRCUIT RACING PUZZLE", TEXT_COLOR_YELLOW);

            /* Ship ASCII art (simplified) */
            text_draw(11, 9, "/|\\", TEXT_COLOR_WHITE);
            text_draw(12, 10, "===", TEXT_COLOR_WHITE);

            /* Instructions */
            text_draw(5, 13, "PRESS START", TEXT_COLOR_WHITE);

            /* Blink "PRESS START" */
            if ((title_timer / 30) & 1) {
                text_draw(5, 13, "PRESS START", TEXT_COLOR_YELLOW);
            }

            text_draw(4, 16, "2024 GRAVITY LAB", TEXT_COLOR_DARK);
            text_draw(2, 17, "PILOT YOUR SHIP", TEXT_COLOR_DARK);
            text_draw(1, 18, "THROUGH THE CIRCUITS!", TEXT_COLOR_DARK);
            break;

        case STATE_LEVEL_SELECT:
            level_select_render();
            break;

        case STATE_PLAYING:
            /* Render circuit BG scroll */
            circuit_render();

            /* Render obstacles */
            obstacle_render_all();

            /* Render ship */
            ship_render();

            /* Render particles */
            particle_render_all();

            /* Render checkpoint effects */
            checkpoint_render();

            /* Render HUD */
            hud_render();
            break;

        case STATE_EXPLODING:
            /* Keep showing the circuit */
            circuit_render();
            obstacle_render_all();
            explosion_render();
            particle_render_all();
            break;

        case STATE_COMPLETE:
            /* Show completion screen */
            text_clear();

            text_draw(6, 2, "CIRCUIT CLEAR!", TEXT_COLOR_GREEN);

            /* Time */
            text_draw(5, 5, "TIME:", TEXT_COLOR_WHITE);
            text_draw_time(11, 5, completion_time, TEXT_COLOR_YELLOW);

            /* Medal */
            text_draw(5, 8, "MEDAL:", TEXT_COLOR_WHITE);
            const char *mname = medal_get_name(completion_medal);
            u8 mcolor = medal_get_color(completion_medal);
            text_draw(12, 8, mname, mcolor);

            /* Medal sprite */
            if (completion_medal != MEDAL_NONE) {
                medal_render(completion_medal, 120, 80);
            }

            /* Best time */
            u32 best = gt_load_best(pending_level);
            text_draw(5, 11, "BEST:", TEXT_COLOR_WHITE);
            if (best > 0) {
                text_draw_time(11, 11, best, TEXT_COLOR_CYAN);
            } else {
                text_draw(11, 11, "--:--.--", TEXT_COLOR_DARK);
            }

            /* Instructions */
            if (complete_timer > 60) {
                u8 color = (complete_timer / 20) & 1 ? TEXT_COLOR_WHITE : TEXT_COLOR_YELLOW;
                text_draw(4, 16, "PRESS A TO CONTINUE", color);
            }

            particle_render_all();
            break;

        case STATE_PAUSED:
            /* Dim screen effect — just overlay text */
            hud_render();

            /* Pause text */
            if ((pause_blink / 15) & 1) {
                text_draw(8, 8, "PAUSED", TEXT_COLOR_YELLOW);
            }
            text_draw(4, 11, "START:RESUME", TEXT_COLOR_WHITE);
            text_draw(4, 12, "SELECT:QUIT", TEXT_COLOR_WHITE);
            break;
    }

    /* Flush sprites to OAM */
    sprite_update_all();
}

/* -------------------------------------------------------
 *  game_get_state — Current state query
 * ------------------------------------------------------- */
u8 game_get_state(void) {
    return game_state;
}