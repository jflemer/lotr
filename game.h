/****************************************************************************

    game.h
    manages the gaming system


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


#ifndef _GAME_H
#define _GAME_H

#include "character.h"
#include "spot.h"

/* starts a new game */
extern void game_new(void);

/* save game */
extern int game_save(int n);

/* load game */
extern int game_load(int n);

/* activate party */
extern void game_party_activate(int id);


/* proceed to the next game frame */
extern void game_next_frame(void);


/* draws the map */
extern void game_draw_map(void);


/* init the graphics */
extern void game_init_graphics(void);


/* get leader character */
extern Character *game_get_leader();

/* set leader character */
extern int game_set_leader(Character * character);


/* set how much siver pennies has the group */
extern void game_set_silver(int amount);

/* get how much siver pennies has the group */
extern int game_get_silver();

/* group pays silver */
extern int game_pay_silver(int amount);

/* group gets silver */
extern void game_add_silver(int amount);


/* returns game text */
extern char *game_get_text(int index);

/* returns game paragraph text */
extern char *game_get_paragraph_text(int index, int question);


/* is character in the party? */
extern int game_in_party(Character * character);

/* get the game party */
extern int game_get_party(int codes[10]);

/* get the game party characters */
extern int game_get_party_characters(Character * party[11]);

/* set game party characters */
extern void game_set_party_characters(Character * party[11], int size);


/* loads a map */
extern void game_load_map(int map);


/* recruit character to the party */
extern int game_recruit(Character * character, int force);

/* dismiss character */
extern int game_dismiss(Character * character);

/* teleport leader return 1 if the map was changed */
extern int game_leader_teleport(int rel, int x, int y, int dir, int map);

/* gets actual game spot */
extern CommandSpot *game_get_actual_spot(void);

/* continue command spot */
extern void game_continue_spot(void);


/* get game register */
extern int game_get_register(int index);

/* set game register */
extern void game_set_register(int index, int value);

/* set game timer */
extern void game_set_timer(int ticks);

/* set game tmp leader */
extern void game_set_tmp_leader(Character * tmp_leader);

/* dismiss game tmp leader */
extern void game_dismiss_tmp_leader(void);

/* all party characters follow the leader */
extern void game_follow_leader(void);

/* check if we have torch lit */
extern void game_check_light(void);

/* set if characters can move */
extern void game_set_moving(int moving);

/* get if characters can move */
extern int game_get_moving(void);

/* set if party is following leader */
extern void game_set_follow(int follow);

#ifdef TTT
/* convert vol. I characters to vol. II */
extern void game_convert(int game_id);
#endif


#endif /* _GAME_H */
