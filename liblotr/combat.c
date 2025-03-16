/****************************************************************************

    combat.c
    combat system


    Lord of the Rings game engine

    Copyright (C) 2003  Michal Benes

    Lord of the Rings game engine is free software;
    you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this code; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/


#include "lotr.h"
#include "character.h"
#include "cartoon.h"
#include "combat.h"
#include "game.h"
#include "gui.h"
#include "lotr_sdl.h"
#include "map.h"
#include "object.h"
#include "sound.h"
#include "timing.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define COMBAT_MOVE_COST 15
#define COMBAT_ATTACK_COST 30


#define COMBAT_MAX_ENEMIES 10

Character *combat_enemies[COMBAT_MAX_ENEMIES];
int combat_enemies_num = 0;

Character *combat_party[LOTR_PARTY_SIZE];
int combat_party_size;

Character *active_character;

int combat_mode = 0;


/* size of combat area */
#define COMBAT_WIDTH  38
#define COMBAT_HEIGHT 16
#define COMBAT_IN_AREA(xx,yy) ((xx) >= 0 && (xx) < COMBAT_WIDTH && (yy) >= 0 && (yy) < COMBAT_HEIGHT)

/* center of combat area */
#define COMBAT_C_X    19
#define COMBAT_C_Y     8


#define COMBAT_AREA_BLOCKED 0
#define COMBAT_AREA_MOVE 1
#define COMBAT_AREA_OPEN 1000

#define COMBAT_PARTY_AP(c) \
    ((c && c->life >= 6 && combat_character_in_bounds(c)) ? c->ap : -1)


int combat_area[COMBAT_WIDTH][COMBAT_HEIGHT];
int combat_stack[COMBAT_WIDTH * COMBAT_HEIGHT][2];
int combat_stack_start;
int combat_stack_size;

int combat_x, combat_y;
int combat_leader_x;
int combat_leader_y;


char combat_result_text[1024];

int ring_not_working;


static inline
int combat_character_in_bounds(const Character* character)
{
    int x = character->x / 4 - combat_x;
    int y = character->y / 4 - combat_y;
    return COMBAT_IN_AREA(x, y);
}


/*
  combat was loosed
*/

void
combat_loosed(void)
{
    combat_mode = 0;
    fprintf(stderr, "lotr: combat - lost\n");
    gui_died_show
        ("Entire party is dead. Sauron has finally recovered the Ring. Your quest is Over.",
         TRUE);
}



/*
  combat was won
*/

void
combat_done(void)
{
    int i;
    int leader_x, leader_y;

    fprintf(stderr, "lotr: combat - done\n");
    combat_mode = 0;

    combat_party_size = game_get_party_characters(combat_party);
    if (combat_party_size == 0)
    {
        combat_loosed();
        return;
    }

    if (combat_character_in_bounds(game_get_leader())) {
        leader_x = game_get_leader()->x;
        leader_y = game_get_leader()->y;
    } else {
        leader_x = combat_leader_x;
        leader_y = combat_leader_y;
    }

    for (i = 0; i < combat_party_size; ++i) {
        fprintf(stderr, "lotr: combat - done, restoring %s\n", combat_party[i]->name);
        if (combat_party[i]->life < 6)
            combat_party[i]->life = 6;
        if (!combat_character_in_bounds(combat_party[i])) {
            combat_party[i]->x = leader_x;
            combat_party[i]->y = leader_y;
        }
        map_unique_add_character(combat_party[i]);
    }

    if (combat_enemies_num == 0)
    {
        music_combat_won();
    }
    else
    {
        for (i = 0; i < combat_enemies_num; ++i)
        {
            free(combat_enemies[i]);
            combat_enemies[i] = NULL;
        }
        combat_enemies_num = 0;
    }

    map_character_update(game_get_leader());

    quit_menu();
}



/*
  remove enemy
 */

void
combat_enemy_remove(Character *character)
{
    int i;
    for (i = 0; i < combat_enemies_num; ++i)
        if (combat_enemies[i] == character)
            break;

    if (i == combat_enemies_num)
        return;

    map_remove_character(character->id);
    free(character);
    combat_enemies[i] = NULL;

    for (; i + 1 < combat_enemies_num; ++i)
        combat_enemies[i] = combat_enemies[i + 1];

    combat_enemies_num--;

    for (; i < COMBAT_MAX_ENEMIES; ++i)
        combat_enemies[i] = NULL;

}




/*
  remove character
 */

void
combat_character_remove(Character *character)
{
    int i;

    character->ap = 0;
    character->action = CHARACTER_STAY;

    if (!game_in_party(character)) {
        combat_enemy_remove(character);
        return;
    }


    for (i = 0; i < combat_party_size; ++i)
        if (combat_party[i] == character)
            break;

    if (i == combat_party_size)
        return;

    map_remove_character(character->id);
    combat_party[i] = NULL;

    for (; i + 1 < combat_party_size; ++i)
        combat_party[i] = combat_party[i + 1];

    combat_party_size--;

    for (; i < LOTR_PARTY_SIZE; ++i)
        combat_party[i] = NULL;

}


void
combat_character_killed(Character *character)
{
    fprintf(stderr, "lotr: combat - %s killed\n", character->name);
    combat_character_remove(character);
    if (game_in_party(character)) {
        fprintf(stderr, "lotr: combat - %s dismissed\n", character->name);
        gui_player_dead(character, 1);
        game_dismiss(character);
    }
    if (combat_party_size == 0 && game_get_party(NULL) == 0)
        fprintf(stderr, "lotr: combat - all dead or out cold\n");
}



/*
  next combat turn
*/

void
combat_next_turn(void)
{
    int i, max_action = -1;
    int active_chars;

    fprintf(stderr, "lotr: combat - next turn\n");

    for (i = 0; i < combat_enemies_num; ++i) {
        if (combat_enemies[i]->life <= 0) {
            combat_character_killed(combat_enemies[i]);
            i = 0;
        }
    }
    if (combat_enemies_num <= 0) {
        combat_done();
        return;
    }

    active_chars = 0;
    for (i = 0; i < combat_party_size; ++i) {
        if (combat_party[i]->life < 6) {
            combat_party[i]->life--;
            fprintf(stderr, "lotr: combat - %s life drops to %d\n", combat_party[i]->name, combat_party[i]->life);
        } else if (combat_character_in_bounds(combat_party[i])) {
            active_chars++;
        }
    }

    for (i = 0; i < combat_party_size; ++i) {
        if (combat_party[i]->life <= 0 || active_chars <= 0) {
            combat_character_killed(combat_party[i]);
            i = 0;
        }
    }

    active_character = NULL;
    for (i = 0; i < combat_party_size; ++i) {
        if (combat_party[i]->life < 6 || !combat_character_in_bounds(combat_party[i])) {
            combat_party[i]->ap = -1;
        } else {
            combat_party[i]->ap = combat_party[i]->dex;
        }
        fprintf(stderr, "lotr: combat - character: %s - ap %d\n", combat_party[i]->name, combat_party[i]->ap);
        if (combat_party[i]->ap > max_action) {
            max_action = combat_party[i]->ap;
            active_character = combat_party[i];
        }
        while (combat_party[i]->action != CHARACTER_STAY)
        {
            character_frame(combat_party[i]);
            map_display(0, 0);
        }
    }

    for (i = 0; i < combat_enemies_num; ++i) {
        combat_enemies[i]->ap = combat_enemies[i]->dex;
        fprintf(stderr, "lotr: combat - enemy: %s - ap %d\n", combat_enemies[i]->name, combat_enemies[i]->ap);
        if (combat_enemies[i]->ap > max_action) {
            max_action = combat_enemies[i]->ap;
            active_character = combat_enemies[i];
        }
    }

    if (active_character) {
        fprintf(stderr, "lotr: combat - active character: %s (ap %d)\n", active_character->name, active_character->ap);
        gui_set_choosed(active_character);
    } else {
        combat_done();
    }
}

/*
   init combat area
*/

void
combat_area_init(void)
{
    int i, j;
    int x, y;

    for (i = 0; i < COMBAT_WIDTH; ++i)
        for (j = 0; j < COMBAT_HEIGHT; ++j) {
            x = combat_x + i;
            y = combat_y + j;
            if (terrain_free(x, y + 1) &&
                terrain_free(x + 1, y + 1) && terrain_free(x + 2, y + 1))
                combat_area[i][j] = COMBAT_AREA_OPEN;
            else
                combat_area[i][j] = COMBAT_AREA_BLOCKED;
        }
}







/*
  finds distances in the combat_area
 */

void
combat_area_compute_distances(void)
{
    int x, y, d;
    while (combat_stack_start < combat_stack_size) {
        x = combat_stack[combat_stack_start][0];
        y = combat_stack[combat_stack_start][1];
        if (COMBAT_IN_AREA(x, y))
            d = combat_area[x][y] + 1;
        else
            d = COMBAT_AREA_MOVE;
#define UPDATE(xx,yy,dd) \
        if (COMBAT_IN_AREA(xx, yy) && combat_area[xx][yy] > dd) { \
            combat_area[xx][yy] = dd; \
            combat_stack[combat_stack_size][0] = xx; \
            combat_stack[combat_stack_size][1] = yy; \
            combat_stack_size++; \
        }
        UPDATE(x - 1, y, d);
        UPDATE(x, y - 1, d);
        UPDATE(x + 1, y, d);
        UPDATE(x, y + 1, d);
#undef UPDATE
        combat_stack_start++;
    }
}



/*
  moves character to the same area as leader
*/

void
combat_move_to_area(Character *character)
{
    int xx = -1, yy = -1, d;
    int x, y;
    int i, j;

    x = character->x / 4 - combat_x;
    y = character->y / 4 - combat_y;

    if (!combat_character_in_bounds(character)) {
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        if (x >= COMBAT_WIDTH)
            x = COMBAT_WIDTH - 1;
        if (y >= COMBAT_HEIGHT)
            x = COMBAT_HEIGHT - 1;
    } else {
        /* character can reach target */
        if (combat_area[x][y] != COMBAT_AREA_BLOCKED && combat_area[x][y] < COMBAT_AREA_OPEN)
            return;
    }

    d = COMBAT_AREA_OPEN;
    for (i = 0; i < COMBAT_WIDTH; ++i) {
        for (j = 0; j < COMBAT_HEIGHT; ++j) {
            if (combat_area[i][j] == COMBAT_AREA_BLOCKED || combat_area[i][j] == COMBAT_AREA_OPEN)
                continue;
            if (abs(i - x) + abs(j - y) < d) {
                d = abs(i - x) + abs(j - y);
                xx = i;
                yy = j;
            }
        }
    }

    if (xx < 0) {
        character->x = game_get_leader()->x;
        character->y = game_get_leader()->y;
        fprintf(stderr, "lotr: combat - moved %s to leader\n", character->name);
    } else {
        character->x = (combat_x + xx) * 4;
        character->y = (combat_y + yy) * 4;
        combat_area[xx][yy] = COMBAT_AREA_BLOCKED;
        fprintf(stderr, "lotr: combat - moved %s near leader\n", character->name);
    }

    map_character_update(character);
    /* we must do it, otherwise map_can_move_to() does not work */
    map_display(0, 0);

}



/*
  start combat
*/

void
combat_start(void)
{

    int i, j;
    int leader_x, leader_y;

    fprintf(stderr, "lotr: combat - start\n");
    combat_mode = 1;
    music_combat_started();
    quit_menu();
    combat_result_text[0] = '\0';

    map_get_center(&combat_x, &combat_y);

    combat_x /= 8;
    combat_y /= 8;

    combat_x -= COMBAT_C_X;
    combat_y -= COMBAT_C_Y;


    combat_party_size = game_get_party_characters(combat_party);

    combat_leader_x = game_get_leader()->x;
    leader_x = combat_leader_x / 4 - combat_x;
    combat_leader_y = game_get_leader()->y;
    leader_y = combat_leader_y / 4 - combat_y;

    if (leader_x < 0)
        leader_x = 0;
    if (leader_y < 0)
        leader_y = 0;
    if (leader_x >= COMBAT_WIDTH)
        leader_x = COMBAT_WIDTH - 1;
    if (leader_y >= COMBAT_HEIGHT)
        leader_x = COMBAT_HEIGHT - 1;


    combat_area_init();

    combat_area[leader_x][leader_y] = COMBAT_AREA_MOVE;
    combat_stack[0][0] = leader_x;
    combat_stack[0][1] = leader_y;
    combat_stack_size = 1;
    combat_stack_start = 0;
    combat_area_compute_distances();


    for (i = 0; i < combat_party_size; ++i)
        combat_move_to_area(combat_party[i]);

    ring_not_working = 0;

    for (i = 0; i < combat_enemies_num; ++i) {
        /* somewhat stupid */
        if (!strcmp(combat_enemies[i]->name, "Nazgul") ||
            !strcmp(combat_enemies[i]->name, "Witch-King") ||
            !strcmp(combat_enemies[i]->name, "Ghostking") ||
            !strcmp(combat_enemies[i]->name, "Ghost") ||
            !strcmp(combat_enemies[i]->name, "Balrog") ||
            !strcmp(combat_enemies[i]->name, "Barrow Wight") ||
            !strcmp(combat_enemies[i]->name, "Werewolf"))
            ring_not_working = 1;

        if (strcmp(combat_enemies[i]->name, "Tentacles"))
            combat_move_to_area(combat_enemies[i]);

        for (j = 0; j < combat_enemies[i]->items_num; ++j)
            combat_enemies[i]->item_used[j] = 0;

        for (j = 0; j < combat_enemies[i]->items_num; ++j)
            if (object_is_weapon(combat_enemies[i]->items[j])) {
                combat_enemies[i]->item_used[j] = 1;
                break;
            }

        for (j = 0; j < combat_enemies[i]->items_num; ++j)
            if (object_is_armour(combat_enemies[i]->items[j])) {
                combat_enemies[i]->item_used[j] = 1;
                break;
            }

        for (j = 0; j < combat_enemies[i]->items_num; ++j)
            if (object_is_shield(combat_enemies[i]->items[j])) {
                combat_enemies[i]->item_used[j] = 1;
                break;
            }

    }

    combat_next_turn();

}




/*
  genocide monsters
*/

void
combat_genocide(void)
{
    while (combat_enemies_num)
        combat_character_killed(combat_enemies[0]);
}



/*
  add new enemy -- returns new id
*/

int
combat_enemy(Character *character, int x, int y, int dir, int map_id)
{
    Character *new_character;

    if (combat_enemies_num == COMBAT_MAX_ENEMIES) {
        fprintf(stderr, "lotr: Too many enemies!\n");
        exit(1);
    }

    new_character = lotr_malloc(sizeof(Character));
    memcpy(new_character, character, sizeof(Character));

    new_character->id = 0x100 + combat_enemies_num;
    new_character->x = x;
    new_character->y = y;
    new_character->direction = dir;
    new_character->map_id = map_id;

    combat_enemies[combat_enemies_num++] = new_character;

    fprintf(stderr, "lotr: combat - add enemy: %s (id %d)\n", new_character->name, new_character->id);
    return new_character->id;
}



/*
  return enemy
*/

Character *
combat_get_enemy(int id)
{
    int i;
    for (i = 0; i < combat_enemies_num; ++i)
        if (combat_enemies[i]->id == id)
            return combat_enemies[i];

    return NULL;
}





/*
  is combat mode?
*/

int
combat_get_mode()
{
    return combat_mode;
}



int combat_select_target()
{
    int j;
    int first_char;
    int is_using_bow;
    int active_chars;

    if (combat_party_size <= 0 || active_character == NULL)
        return -1;

    is_using_bow = character_using_bow(active_character);
    first_char = lotr_rnd(combat_party_size) - 1;

    active_chars = 0;
    for (j = 0; j < combat_party_size; ++j) {
        int i = (first_char + j) % combat_party_size;
        if (combat_party[i]->life < 6 || !combat_character_in_bounds(combat_party[i]))
            continue;
        active_chars++;

        int is_near = abs(active_character->x - combat_party[i]->x) < MAP_NEAR_DISTANCE;
        is_near = is_near && abs(active_character->y - combat_party[i]->y) < MAP_NEAR_DISTANCE;

        if ((is_near || is_using_bow)
            && (combat_party[i]->ring_mode != 2 || ring_not_working))
        {
            fprintf(stderr, "lotr: combat - %s targets %s\n", active_character->name, combat_party[i]->name);
            return i;
        }
    }

    return active_chars ? -1 : -2;
}


int
combat_check_in_bounds()
{
    if (!combat_character_in_bounds(active_character))
    {
        if (game_in_party(active_character)) {
            snprintf(combat_result_text, sizeof(combat_result_text), "%s has escaped.",
                     active_character->name);
        } else {
            snprintf(combat_result_text, sizeof(combat_result_text), "%s abandons the battle.",
                     active_character->name);
        }
        fprintf(stderr, "lotr: combat - %s\n", combat_result_text);
        return 0;
    }
    return 1;
}


/*
  player moves
 */


void
combat_player_move()
{

    if (lotr_key_left() &&
        map_can_move_to(active_character, active_character->x / 4 - 1,
                        active_character->y / 4)) {
        character_move_left(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
    }

    else if (lotr_key_right() &&
        map_can_move_to(active_character, active_character->x / 4 + 1,
                        active_character->y / 4)) {
        character_move_right(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
    }

    else if (lotr_key_up() &&
        map_can_move_to(active_character, active_character->x / 4,
                        active_character->y / 4 - 1)) {
        character_move_up(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
    }

    else if (lotr_key_down() &&
        map_can_move_to(active_character, active_character->x / 4,
                        active_character->y / 4 + 1)) {
        character_move_down(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
    }

}





/*
  enemy moves
 */


void
combat_enemy_move()
{

    int i, v, x, y;
    int leave;

    /* choose whom to attack */

    i = combat_select_target();

    if (i >= 0) {
        combat_attack(active_character, combat_party[i]);
        return;
    }

    fprintf(stderr, "lotr: combat - %s moves\n", active_character->name);


    /* choose a shortest path to our opponent */

    combat_area_init();
    for (i = 0; i < combat_enemies_num; ++i) {
        if (active_character == combat_enemies[i])
            continue;
        x = combat_enemies[i]->x / 4 - combat_x;
        y = combat_enemies[i]->y / 4 - combat_y;
        if (COMBAT_IN_AREA(x, y))
            combat_area[x][y] = COMBAT_AREA_BLOCKED;
    }

    combat_stack_size = 0;
    combat_stack_start = 0;
    leave = 0;

    for (i = 0; i < combat_party_size; ++i) {
        if (!combat_character_in_bounds(combat_party[i]) ||
            (combat_party[i]->ring_mode == 2 && !ring_not_working))
            continue;
        x = combat_party[i]->x / 4 - combat_x;
        y = combat_party[i]->y / 4 - combat_y;
        if (COMBAT_IN_AREA(x, y))
            combat_area[x][y] = COMBAT_AREA_BLOCKED;
        if (combat_party[i]->life < 6)
            continue;
        combat_stack[combat_stack_size][0] = x;
        combat_stack[combat_stack_size][1] = y;
        combat_stack_size++;
    }

    if (combat_stack_size == 0) {
        x = active_character->x / 4 - combat_x;
        y = active_character->y / 4 - combat_y;
        combat_stack[combat_stack_size][0] = -1;
        combat_stack[combat_stack_size][1] = y;
        combat_stack_size++;
        combat_stack[combat_stack_size][0] = COMBAT_WIDTH;
        combat_stack[combat_stack_size][1] = y;
        combat_stack_size++;
        combat_stack[combat_stack_size][0] = x;
        combat_stack[combat_stack_size][1] = -1;
        combat_stack_size++;
        combat_stack[combat_stack_size][0] = x;
        combat_stack[combat_stack_size][1] = COMBAT_HEIGHT;
        combat_stack_size++;
        active_character->ap = COMBAT_MOVE_COST * (COMBAT_WIDTH + COMBAT_HEIGHT + 1);
	leave = 1;
    }

    combat_area_compute_distances();




    x = active_character->x / 4 - combat_x;
    y = active_character->y / 4 - combat_y;

    if (COMBAT_IN_AREA(x, y))
        v = combat_area[x][y];
    else
        v = COMBAT_AREA_OPEN;

#define IS_CLOSER(xx,yy) \
    ((COMBAT_IN_AREA(xx,yy) && \
     combat_area[xx][yy] < v && \
     map_can_move_to(active_character, combat_x + xx, combat_y + yy)) \
     || \
     (leave && !COMBAT_IN_AREA(xx,yy)))

    if (IS_CLOSER(x - 1, y)) {
        character_move_left(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

    if (IS_CLOSER(x + 1, y)) {
        character_move_right(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

    if (IS_CLOSER(x, y - 1)) {
        character_move_up(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

    if (IS_CLOSER(x, y + 1)) {
        character_move_down(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

#undef IS_CLOSER


    /* we do not know what to do */
    active_character->ap = 0;

}


/*
  combat mode frame
*/

int
combat_frame()
{
    int finished = 0;

    if (!combat_mode)
        return 0;

    if (active_character->action != CHARACTER_STAY) {
        character_frame(active_character);
        map_display(0, 0);
    }

    else if (gui_mode() == DIALOG_MESSAGE) {
        if (gui_frame() != DIALOG_MESSAGE) {
            finished = 1;
        }
    }

    else if (combat_result_text[0]) {
        gui_message(combat_result_text, 1);
	combat_result_text[0] = '\0';
    }

    else if (!combat_check_in_bounds()) {
        gui_message(combat_result_text, 1);
	combat_result_text[0] = '\0';
    }

    else if (active_character->ap <= 0) {
        finished = 1;
    }

    else {
        if (game_in_party(active_character)) {
            if (gui_frame() == MAIN_MENU && game_get_moving()) {
                combat_player_move();
            }
        } else {
            combat_enemy_move();
        }
    }

    map_animate_frame();

    if (finished)
        combat_character_finished();

    return combat_mode;
}








/*
  combat character finished turn
*/

void
combat_character_finished()
{
    int i, max_action = 0;

    fprintf(stderr, "lotr: combat - %s turn finished\n", active_character->name);

    if (active_character->ap < 0)
        active_character->ap = 0;

    while (active_character->action != CHARACTER_STAY) {
        character_frame(active_character);
        map_display(0, 0);
    }

    if (!combat_character_in_bounds(active_character))
        combat_character_remove(active_character);

    active_character = NULL;
    for (i = 0; i < combat_party_size; ++i)
        if (combat_party[i]->life >= 6 && combat_party[i]->ap > max_action) {
            max_action = combat_party[i]->ap;
            active_character = combat_party[i];
        }

    for (i = 0; i < combat_enemies_num; ++i)
        if (combat_enemies[i]->ap > max_action) {
            max_action = combat_enemies[i]->ap;
            active_character = combat_enemies[i];
        }

    lotr_reset_keyboard();
    if (active_character && combat_enemies_num > 0) {
        fprintf(stderr, "lotr: combat - next active character: %s\n", active_character->name);
        gui_set_choosed(active_character);
    } else {
        combat_next_turn();
    }


}




/*
  hits who whom?
*/

int
combat_if_hits(int who_dex, int whom_dex, int tohit)
{
    if (tohit > 0)
        who_dex += tohit * 10;
    else
        whom_dex -= tohit * 10;

    if (lotr_rnd(who_dex) > lotr_rnd(whom_dex))
        return 1;

    return 0;

}



/*
  get enemies in bow range
*/

int
combat_in_bow_range(Character *character, int codes[10])
{
    int i;

    if (!character_using_bow(character))
        return 0;

    for (i = 0; i < combat_enemies_num; ++i)
        codes[i] = combat_enemies[i]->id;

    return combat_enemies_num;

}




/*
  character attack another character
 */


void
combat_attack(Character *who, Character *whom)
{

    int dir;
    int tohit, damage, damage1, damage2, damage_reduced;
    int hits, hits2, luck1, luck2;
    int i;
    int item, weapon_skill;
    char *attack_type;
    int bonus;

    who->ap -= COMBAT_ATTACK_COST;

    dir = map_toward(who->x, who->y, whom->x, whom->y);

    character_attack(who, dir);



    tohit = 0;

    if (game_in_party(who))
        damage = 4;             /* bare hand */
    else
        damage = 6;             /* something better */

    attack_type = "swings";

    /* when unarmed, brawl is in effect */
    weapon_skill = 0x40;

    bonus = 0;

    for (i = 0; i < who->items_num; ++i)
        if (who->item_used[i]) {
            item = who->items[i];
            if (object_is_weapon(item)) {
                weapon_skill = object_weapon_skill(item);
                damage = object_weapon_damage(item);
                tohit += object_weapon_to_hit(item);
                if (object_weapon_class(item) == 8)     /* bow */
                    attack_type = "fires";

                /* Sting, Glamdring, Anduril, Durin's Axe, Magic Bow */
                if (item == 0x60 || item == 0x63 || item == 0x62
                    || item == 0x67 || item == 0x65)
                    bonus = 1;

                /* Spider Sword */
                if (item == 0x64 && strcmp(whom->name, "Spider"))
                    bonus = 1;

                /* Troll Slayer */
                if (item == 0x66 &&
                    (strcmp(whom->name, "Troll") ||
                     strcmp(whom->name, "Olog-hai")))
                    bonus = 1;

            }

            /* armours have impairing effect */
            if (object_is_armour(item))
                tohit -= object_weapon_to_hit(item);
        }

    /* better chance to hit if we are skilled with our weapon */
    for (i = 0; i < who->skills_num; ++i)
        if (weapon_skill == who->skills[i]) {
            tohit += 2;
            break;
        }

    damage_reduced = 0;

    for (i = 0; i < whom->items_num; ++i)
        if (whom->item_used[i]) {
            item = who->items[i];

            /* armours have impairing effect */
            if (object_is_armour(item)) {
                damage_reduced = object_armour_reduced(item);
                tohit += object_weapon_to_hit(item);
            }

            if (object_is_shield(item))
                tohit -= object_shield_to_hit(item);
        }


    /* better chance to defend if we have dodge */
    for (i = 0; i < whom->skills_num; ++i)
        if (whom->skills[i] == 0x46) {
            tohit -= 2;
            break;
        }


    hits = combat_if_hits(who->dex, whom->dex, tohit);
    hits2 = combat_if_hits(who->dex, whom->dex, tohit);

    luck1 = lotr_rnd(30 + who->luck);
    luck2 = lotr_rnd(30 + who->luck);

    if (luck1 > 2 * luck2)
        hits = max(hits, hits2);

    if (2 * luck1 < luck2)
        hits = min(hits, hits2);


    /* Balrog must be a deadly foe */
    if (!strcmp(who->name, "Balrog")) {
        hits = 1;
        bonus = 1;
    }

    if (hits) {
        damage1 = lotr_rnd(who->str) / 10 + lotr_rnd(damage)
            + bonus * lotr_rnd(damage) - damage_reduced;
        damage2 = lotr_rnd(who->str) / 10 + lotr_rnd(damage)
            + bonus * lotr_rnd(damage) - damage_reduced;


        luck1 = lotr_rnd(30 + who->luck);
        luck2 = lotr_rnd(30 + who->luck);

        damage = damage1;

        if (luck1 > 2 * luck2)
            damage = max(damage1, damage2);

        if (2 * luck1 < luck2)
            damage = min(damage1, damage2);

        if (damage < 0)
            damage = 0;

        whom->life -= damage;
        if (whom->life < 0)
            whom->life = 0;

    }


    if (hits) {
        if (damage > 0) {
            if (whom->life == 0) {
                snprintf(combat_result_text, sizeof(combat_result_text), "%s kills %s.", who->name,
                         whom->name);
                whom->ap = 0;
                combat_character_killed(whom);

            } else {
                if (whom->life >= 6 || !game_in_party(whom)) {
                    snprintf(combat_result_text, sizeof(combat_result_text),
                             "%s hits %s for %d points of damage.", who->name,
                             whom->name, damage);
                } else {
                    snprintf(combat_result_text, sizeof(combat_result_text),
                             "%s hits %s for %d points of damage, knocking %s out of cold.",
                             who->name, whom->name, damage, whom->name);
                    whom->ap = 0;
                    fprintf(stderr, "lotr: combat - %s knocked out\n", whom->name);
                }
            }
        } else {
            snprintf(combat_result_text, sizeof(combat_result_text), "%s hits %s but does no damage.",
                     who->name, whom->name);
        }
    } else {
        snprintf(combat_result_text, sizeof(combat_result_text), "%s %s at and misses %s.", who->name,
                 attack_type, whom->name);
    }
    fprintf(stderr, "lotr: combat - %s\n", combat_result_text);

    if (hits)
        sound_play(10);
    else
        sound_play(11);

}



/*
  proceed all combat frames
*/
void
combat_proceed_frames(void)
{
    while (combat_frame()) {
        map_animate_frame();
        graphics_update_screen();
        lotr_timer(FRAME_TIME);
        lotr_poll_events();
    }
}
