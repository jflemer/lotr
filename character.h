/****************************************************************************

    character.h
    game character (playable & non-playable)


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


#ifndef _CHARACTER_H
#define _CHARACTER_H


#include "shape.h"
#include <stdio.h>
#include <libxml/parser.h>


#define CHARACTER_UP          0
#define CHARACTER_RIGHT       1
#define CHARACTER_DOWN        2
#define CHARACTER_LEFT        3
#define CHARACTER_ATTACK      4
#define CHARACTER_STAY        5


#ifdef CD_VERSION
#define CD_NORTH   1
#define CD_EAST    2
#define CD_SOUTH   4
#define CD_WEST    8
#endif

#define CHARACTERS_NUM     240

#define CHARACTER_TALK_LEN 169

typedef struct
{
    int x, y;
    int next_x, next_y;
    int map;
    int display_x, display_y;
    int direction;
    int action;
    int state;
    int shape_id;
    int horse_shape_id;
    Shape *shapes[CHARACTER_STAY + 1];
    int weapon_shape_id;
    int portrait;
    int dex, end, life, str, luck, will, ap;
    int silver;
    int last_eat;
    int has_bow;
    int items[10];
    int item_used[10];
    int items_num;
    int spells[10];
    int spells_num;
    int skills[10];
    int skills_num;
    int id;
    int map_id;
    int party_id;               /* used in TTT for multiple party implementation */
    int willing_join;
    int ring_mode;
    int actived;
    int converted;
    char name[20];
    char original_name[20];
    char texts[CHARACTER_TALK_LEN];
    int pythontexts;
} Character;



/* init characters */
extern void characters_init();

/* close characters */
extern void characters_close();

/* save characters */
extern void characters_save(xmlNodePtr node);

/* load characters */
extern void characters_load(xmlNodePtr node);

#ifdef TTT
/* convert character */
extern void character_convert(xmlNodePtr node, int index);
#endif

/* character type exists*/
extern int character_exists(int i);

/* get character type */
extern Character *character_get(int i);



/* put characters to the map */
extern void character_fill_map(int map_num, int building);

/* draw character */
extern void character_draw(int index, int x, int y, int dir);


/* set character attack */
extern void character_attack(Character *character, int direction);


/* set characters moving direction */
extern void character_move_left(Character *character);
extern void character_move_right(Character *character);
extern void character_move_up(Character *character);
extern void character_move_down(Character *character);

/* next characters frame */
extern void character_frame(Character *character);


/* discards character item */
extern void character_discard_item(Character *character, int index);

/* adds character item */
extern int character_add_item(Character *character, int item);

/* adds character spell */
extern int character_add_spell(Character *character, int item);

/* adds character skill */
extern int character_add_skill(Character *character, int item);

/* perform COMMAND_NPC_INIT */
extern void character_command_npc_init(Character *character, int type,
                                       int value);

/* let character use object */
extern void character_use(Character *character, int index);

/* get ringbearer */
extern Character *character_get_ringbearer();

/* set party with given identifier */
extern void character_set_party(int id);

/* get CHARACTER_TALK_LEN */
extern int character_get_talk_len(void);


#endif /* _CHARACTER_H */
