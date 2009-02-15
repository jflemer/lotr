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


#include "lord.h"
#include "character.h"
#include "cartoon.h"
#include "combat.h"
#include "game.h"
#include "gui.h"
#include "lord_sdl.h"
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

Character *combat_party[11];
int combat_party_size;

Character *active_character;

int combat_mode = 0;


/* size of combat area */
#define COMBAT_WIDTH  38
#define COMBAT_HEIGHT 16

/* center of combat area */
#define COMBAT_C_X    19
#define COMBAT_C_Y     8


int combat_area[COMBAT_WIDTH][COMBAT_HEIGHT];
int combat_stack[COMBAT_WIDTH * COMBAT_HEIGHT][2];
int combat_stack_start;
int combat_stack_size;

int combat_x, combat_y;
int combat_leader_x;
int combat_leader_y;


/* turned on during attack animation */
int combat_attack_animation;

Character *combat_who_attacked = NULL;

char combat_result_text[1024];

int ring_not_working;

/*
  combat was loosed
*/

void
combat_loosed(void)
{
    combat_mode = 0;
    playcartoon("cart10");
    exit(0);
}



/*
  combat was won
*/

void
combat_won(void)
{
    int i;

    combat_mode = 0;
    music_combat_won();
    game_set_party_characters(combat_party, combat_party_size);

    if (!game_in_party(game_get_leader()))
        game_set_leader(combat_party[0]);

    game_get_leader()->x = combat_leader_x;
    game_get_leader()->y = combat_leader_y;
    map_character_update(game_get_leader());

    for (i = 0; i < combat_party_size; ++i)
        if (combat_party[i]->life < 6)
            combat_party[i]->life = 6;

    quit_menu();
}



/*
  remove enemy
 */

void
combat_enemy_remove(Character * character)
{
    int i;
    for (i = 0; i < combat_enemies_num; ++i)
        if (combat_enemies[i] == character)
            break;

    if (i == combat_enemies_num)
        return;

    map_remove_character(character->id);
    free(character);

    for (; i + 1 < combat_enemies_num; ++i)
        combat_enemies[i] = combat_enemies[i + 1];

    combat_enemies_num--;

    if (combat_enemies <= 0)
        combat_won();

}




/*
  remove character
 */

void
combat_character_remove(Character * character)
{
    int i;

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

    for (; i + 1 < combat_party_size; ++i)
        combat_party[i] = combat_party[i + 1];

    combat_party_size--;

    if (combat_party_size <= 0)
        combat_loosed();


}



/*
  next combat turn
*/

void
combat_next_turn(void)
{
    int i, max_action = -1;
    int active_chars;


    if (combat_party_size <= 0)
        combat_loosed();

    if (combat_enemies_num <= 0)
        combat_won();


    active_chars = 0;
    for (i = 0; i < combat_party_size; ++i) {
        if (combat_party[i]->life < 6) {
            combat_party[i]->life--;
            if (combat_party[i]->life <= 0) {
                combat_character_remove(combat_party[i]);
                gui_player_dead(combat_party[i], 1);
            }
        }
        else {
            active_chars++;
            combat_party[i]->ap = combat_party[i]->dex;
            if (combat_party[i]->ap > max_action) {
                max_action = combat_party[i]->ap;
                active_character = combat_party[i];
            }
            while (combat_party[i]->action != CHARACTER_STAY)
                character_frame(combat_party[i]);
        }
    }

    if (active_chars <= 0)
        combat_loosed();

    for (i = 0; i < combat_enemies_num; ++i) {
        combat_enemies[i]->ap = combat_enemies[i]->dex;
        if (combat_enemies[i]->ap > max_action) {
            max_action = combat_enemies[i]->ap;
            active_character = combat_enemies[i];
        }
    }

    gui_set_choosed(active_character);

    if (combat_enemies_num == 0)
        combat_won();

    active_character->has_bow = 0;

    for (i = 0; i < active_character->items_num; ++i)
        if (active_character->item_used[i] &&
            object_weapon_class(active_character->items[i]) == 8) {
            active_character->has_bow = 1;
            break;
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
                combat_area[i][j] = 1000;
            else
                combat_area[i][j] = 0;
        }
}







/*
  finds distances in the combat_area
 */

void
combat_stack_proceed(void)
{
    int x, y, v;
    while (combat_stack_start < combat_stack_size) {
        x = combat_stack[combat_stack_start][0];
        y = combat_stack[combat_stack_start][1];
        v = combat_area[x][y];
        if (x > 0 && combat_area[x - 1][y] > v + 1) {
            combat_stack[combat_stack_size][0] = x - 1;
            combat_stack[combat_stack_size][1] = y;
            combat_area[x - 1][y] = v + 1;
            combat_stack_size++;
        }
        if (x + 1 < COMBAT_WIDTH && combat_area[x + 1][y] > v + 1) {
            combat_stack[combat_stack_size][0] = x + 1;
            combat_stack[combat_stack_size][1] = y;
            combat_area[x + 1][y] = v + 1;
            combat_stack_size++;
        }
        if (y > 0 && combat_area[x][y - 1] > v + 1) {
            combat_stack[combat_stack_size][0] = x;
            combat_stack[combat_stack_size][1] = y - 1;
            combat_area[x][y - 1] = v + 1;
            combat_stack_size++;
        }
        if (y + 1 < COMBAT_HEIGHT && combat_area[x][y + 1] > v + 1) {
            combat_stack[combat_stack_size][0] = x;
            combat_stack[combat_stack_size][1] = y + 1;
            combat_area[x][y + 1] = v + 1;
            combat_stack_size++;
        }
        combat_stack_start++;
    }
}



/*
  moves character to the same area as leader
*/

void
combat_move_to_area(Character * character)
{
    int xx = -1, yy = -1, d = 1000;
    int x, y;
    int i, j;

    x = character->x / 4 - combat_x;
    y = character->y / 4 - combat_y;

    if (x < 0 || y < 0 || x >= COMBAT_WIDTH || y >= COMBAT_HEIGHT) {
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        if (x >= COMBAT_WIDTH)
            x = COMBAT_WIDTH - 1;
        if (y >= COMBAT_HEIGHT)
            x = COMBAT_HEIGHT - 1;
    }
    else {
        if (combat_area[x][y] != 0 && combat_area[x][y] < 1000)
            return;
    }

    for (i = 0; i < COMBAT_WIDTH; ++i)
        for (j = 0; j < COMBAT_HEIGHT; ++j)
            if (combat_area[i][j] != 0 && combat_area[i][j] < 1000 &&
                map_can_move_to(character, combat_x + i, combat_y + j) &&
                abs(i - x) + abs(j - y) < d) {
                d = abs(i - x) + abs(j - y);
                xx = i;
                yy = j;
            }

    if (xx < 0) {
        character->x = game_get_leader()->x;
        character->y = game_get_leader()->y;
    }
    else {
        character->x = (combat_x + xx) * 4;
        character->y = (combat_y + yy) * 4;
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

    combat_mode = 1;
    music_combat_started();
    quit_menu();
    combat_attack_animation = 0;

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

    combat_area[leader_x][leader_y] = 1;
    combat_stack[0][0] = leader_x;
    combat_stack[0][1] = leader_y;
    combat_stack_size = 1;
    combat_stack_start = 0;
    combat_stack_proceed();


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
            combat_enemies[i]->item_used[i] = 0;

        for (j = 0; j < combat_enemies[i]->items_num; ++j)
            if (object_is_weapon(combat_enemies[i]->items[j])) {
                combat_enemies[i]->item_used[i] = 1;
                break;
            }

        for (j = 0; j < combat_enemies[i]->items_num; ++j)
            if (object_is_armour(combat_enemies[i]->items[j])) {
                combat_enemies[i]->item_used[i] = 1;
                break;
            }

        for (j = 0; j < combat_enemies[i]->items_num; ++j)
            if (object_is_shield(combat_enemies[i]->items[j])) {
                combat_enemies[i]->item_used[i] = 1;
                break;
            }

    }

    active_character = NULL;
    combat_next_turn();

}




/*
  genocide monsters
*/

void
combat_genocide(void)
{
    int i;
    for (i = 0; i < combat_enemies_num; ++i) {
        map_remove_character(combat_enemies[i]->id);
        free(combat_enemies[i]);
    }
    combat_enemies_num = 0;

    combat_won();
}



/*
  add new enemy -- returns new id
*/

int
combat_enemy(Character * character, int x, int y, int dir, int map_id)
{
    Character *new_character;

    if (combat_enemies_num == COMBAT_MAX_ENEMIES) {
        fprintf(stderr, "lord: Too many enemies!\n");
        exit(1);
    }

    new_character = lord_malloc(sizeof(Character));
    memcpy(new_character, character, sizeof(Character));

    new_character->id = 0x100 + combat_enemies_num;
    new_character->x = x;
    new_character->y = y;
    new_character->direction = dir;
    new_character->map_id = map_id;

    combat_enemies[combat_enemies_num++] = new_character;

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



/*
  player moves
 */


void
combat_player_move()
{

    if (lord_key_left() &&
        map_can_move_to(active_character, active_character->x / 4 - 1,
                        active_character->y / 4)) {
        character_move_left(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

    if (lord_key_right() &&
        map_can_move_to(active_character, active_character->x / 4 + 1,
                        active_character->y / 4)) {
        character_move_right(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

    if (lord_key_up() &&
        map_can_move_to(active_character, active_character->x / 4,
                        active_character->y / 4 - 1)) {
        character_move_up(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

    if (lord_key_down() &&
        map_can_move_to(active_character, active_character->x / 4,
                        active_character->y / 4 + 1)) {
        character_move_down(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

}





/*
  enemy moves
 */


void
combat_enemy_move()
{

    int first_char;
    int i, v, x, y;
    int k, l;
    int active_chars;

    if (combat_party_size <= 0)
        combat_loosed();

    /* choose whom to attack */

    first_char = lord_rnd(combat_party_size) - 1;

    i = first_char;
    while (!active_character->has_bow) {

        if (abs(active_character->x - combat_party[i]->x) < MAP_NEAR_DISTANCE
            && abs(active_character->y - combat_party[i]->y) <
            MAP_NEAR_DISTANCE && combat_party[i]->life >= 6
            && (combat_party[i]->ring_mode != 2 || ring_not_working))
            break;

        i = (i + 1) % combat_party_size;

        if (i == first_char) {
            /* nobody founded */
            i = -1;
            break;
        }
    }

    if (i >= 0) {
        combat_attack(active_character, combat_party[i]);
        return;
    }



    /* choose a shortest path to our opponent */

    combat_area_init();

    for (i = 0; i < combat_enemies_num; ++i)
        if (active_character != combat_enemies[i]) {
            x = combat_enemies[i]->x / 4 - combat_x;
            y = combat_enemies[i]->y / 4 - combat_y;
            combat_area[x][y] = 0;
        }

    combat_stack_size = 0;
    combat_stack_start = 0;

    active_chars = 0;
    for (i = 0; i < combat_party_size; ++i)
        if (combat_party[i]->life >= 6) {
            active_chars++;
            if (combat_party[i]->ring_mode == 2 && !ring_not_working)
                continue;
            x = combat_party[i]->x / 4 - combat_x;
            y = combat_party[i]->y / 4 - combat_y;
            combat_area[x][y] = 0;

            for (k = -MAP_NEAR_DISTANCE / 4; k <= MAP_NEAR_DISTANCE / 4; ++k)
                for (l = -MAP_NEAR_DISTANCE / 4; l <= MAP_NEAR_DISTANCE / 4;
                     ++l)
                    if (x + k >= 0 && x + k < COMBAT_WIDTH && y + l >= 0
                        && y + l < COMBAT_HEIGHT
                        && combat_area[x + k][y + l] >= 1000) {
                        combat_area[x + k][y + l] = 1;
                        combat_stack[combat_stack_size][0] = x + k;
                        combat_stack[combat_stack_size][1] = y + l;
                        combat_stack_size++;
                    }
        }

    if (active_chars == 0)
        combat_loosed();

    if (combat_stack_size == 0) {
        active_character->x = -1;
        sprintf(combat_result_text, "%s abandons the battle.",
                active_character->name);
        gui_message(combat_result_text, 1);
        return;
    }

    combat_stack_proceed();




    x = active_character->x / 4 - combat_x;
    y = active_character->y / 4 - combat_y;

    v = combat_area[x][y];

    if (x > 0 &&
        combat_area[x - 1][y] != 0 && combat_area[x - 1][y] < v &&
        map_can_move_to(active_character, combat_x + x - 1, combat_y + y)) {
        character_move_left(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

    if (x + 1 < COMBAT_WIDTH &&
        combat_area[x + 1][y] != 0 && combat_area[x + 1][y] < v &&
        map_can_move_to(active_character, combat_x + x + 1, combat_y + y)) {
        character_move_right(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

    if (y > 0 &&
        combat_area[x][y - 1] != 0 && combat_area[x][y - 1] < v &&
        map_can_move_to(active_character, combat_x + x, combat_y + y - 1)) {
        character_move_up(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }

    if (y + 1 < COMBAT_HEIGHT &&
        combat_area[x][y + 1] != 0 && combat_area[x][y + 1] < v &&
        map_can_move_to(active_character, combat_x + x, combat_y + y + 1)) {
        character_move_down(active_character);
        active_character->ap -= COMBAT_MOVE_COST;
        return;
    }


    /* we do not know what to do */
    active_character->ap = 0;

}


/*
  combat mode frame
*/

int
combat_frame()
{

    if (!combat_mode)
        return 0;

    if (active_character->action != CHARACTER_STAY) {
        character_frame(active_character);
        map_display(0, 0);
        goto end_combat_frame;
    }


    if (gui_mode() == DIALOG_MESSAGE) {
        gui_frame();

        if (gui_mode() != DIALOG_MESSAGE) {

            if (combat_who_attacked->life <= 0 &&
                game_in_party(combat_who_attacked))
                gui_player_dead(combat_who_attacked, 0);

            combat_character_finished();

        }

        goto end_combat_frame;
    }

    if (combat_attack_animation) {
        combat_attack_animation = 0;

        /* message switches to next player */
        gui_message(combat_result_text, 1);
        goto end_combat_frame;
    }

    if (game_in_party(active_character)) {

        if (gui_frame() != MAIN_MENU)
            goto end_combat_frame;

        if (game_get_moving())
            combat_player_move();
    }
    else {
        combat_enemy_move();
        if (active_character->x < 0)
            goto end_combat_frame;
    }


    if (!combat_attack_animation && active_character->ap <= 0)
        combat_character_finished();

    map_display(0, 0);

  end_combat_frame:

    map_animate_frame();

    return combat_mode;
}








/*
  combat character finished turn
*/

void
combat_character_finished()
{
    int i, max_action = -1;
    int active_chars;

    if (active_character->ap < 0)
        active_character->ap = 0;

    if (active_character->x < 0) {
        combat_enemy_remove(active_character);
        if (combat_enemies <= 0)
            return;
    }
    else {
        while (active_character->action != CHARACTER_STAY)
            character_frame(active_character);
    }

    active_chars = 0;

    for (i = 0; i < combat_party_size; ++i)
        if (combat_party[i]->life >= 6) {
            ++active_chars;
            if (combat_party[i]->ap > max_action) {
                max_action = combat_party[i]->ap;
                active_character = combat_party[i];
            }
        }


    if (active_chars <= 0) {
        combat_loosed();
        return;
    }

    if (combat_enemies_num <= 0) {
        combat_won();
        return;
    }

    for (i = 0; i < combat_enemies_num; ++i)
        if (combat_enemies[i]->ap > max_action) {
            max_action = combat_enemies[i]->ap;
            active_character = combat_enemies[i];
        }

    if (max_action <= 0) {
        combat_next_turn();
        return;
    }

    gui_set_choosed(active_character);
    lord_reset_keyboard();

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

    if (lord_rnd(who_dex) > lord_rnd(whom_dex))
        return 1;

    return 0;

}



/*
  get enemies in bow range
*/

int
combat_in_bow_range(Character * character, int codes[10])
{
    int i;

    if (!character->has_bow)
        return 0;

    for (i = 0; i < combat_enemies_num; ++i)
        codes[i] = combat_enemies[i]->id;

    return combat_enemies_num;

}




/*
  character attack another character
 */


void
combat_attack(Character * who, Character * whom)
{

    int dir;
    int tohit, damage, damage1, damage2, damage_reduced;
    int hits, hits2, luck1, luck2;
    int i;
    int item, weapon_skill;
    char *attack_type;
    int bonus;

    who->ap -= COMBAT_ATTACK_COST;

    if (abs(who->x - whom->x) > abs(who->y - whom->y)) {
        if (who->x > whom->x)
            dir = CHARACTER_LEFT;
        else
            dir = CHARACTER_RIGHT;
    }
    else {
        if (who->y > whom->y)
            dir = CHARACTER_UP;
        else
            dir = CHARACTER_DOWN;
    }


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

    luck1 = lord_rnd(30 + who->luck);
    luck2 = lord_rnd(30 + who->luck);

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
        damage1 = lord_rnd(who->str) / 10 + lord_rnd(damage)
            + bonus * lord_rnd(damage) - damage_reduced;
        damage2 = lord_rnd(who->str) / 10 + lord_rnd(damage)
            + bonus * lord_rnd(damage) - damage_reduced;


        luck1 = lord_rnd(30 + who->luck);
        luck2 = lord_rnd(30 + who->luck);

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


    combat_who_attacked = whom;

    if (hits) {
        if (damage > 0) {
            if (whom->life == 0) {
                sprintf(combat_result_text, "%s kills %s.", who->name,
                        whom->name);
                combat_character_remove(whom);

            }
            else {
                if (whom->life >= 6 || !game_in_party(whom)) {
                    sprintf(combat_result_text,
                            "%s hits %s for %d points of damage.", who->name,
                            whom->name, damage);
                }
                else {
                    sprintf(combat_result_text,
                            "%s hits %s for %d points of damage, knocking %s out of cold.",
                            who->name, whom->name, damage, whom->name);
                }
            }
        }
        else {
            sprintf(combat_result_text, "%s hits %s but does no damage.",
                    who->name, whom->name);
        }
    }
    else {
        sprintf(combat_result_text, "%s %s at and misses %s.", who->name,
                attack_type, whom->name);
    }

    if (hits)
        sound_play(10);
    else
        sound_play(11);

    combat_attack_animation = 1;

}



/*
  proceed all combat frames
*/
void
combat_proceed_frames(void)
{
    while (combat_frame()) {
        map_animate_frame();
        UpdateScreen();
        Timer(FRAME_TIME);
        lord_poll_events();
    }
}
