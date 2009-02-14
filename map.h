/****************************************************************************

    map.h
    handling the game map


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


#ifndef _MAP_H
#define _MAP_H

#include "archive.h"
#include "character.h"
#include "spot.h"
#include <libxml/parser.h>


/* map dimens */
#define MAP_WIDTH 64
#define MAP_HEIGHT 64

/* building dimens */
#define BUILDING_WIDTH 32
#define BUILDING_HEIGHT 128

/* map modes */
#define MAP_DARK    0
#define MAP_NORMAL  1
#define MAP_LIGHTED 2
#define MAP_FOG     4

#define MAP_NEAR_DISTANCE 0x11


/* terrain types */

#define TERRAIN_PATH        0
#define TERRAIN_INPASSABLE  1
#define TERRAIN_SWAMP       2
#define TERRAIN_ROCK        3
#define TERRAIN_DOWN        4
#define TERRAIN_UP          5
#define TERRAIN_PIT         6
#define TERRAIN_STAIRS_DOWN 7
#define TERRAIN_WATER2      8

#define TERRAIN_EASY	   10
#define TERRAIN_HARD	   11
#define TERRAIN_HARDER	   12
#define TERRAIN_ICE	   13
#define TERRAIN_ICEPATH	   14
#define TERRAIN_WATER	   15



/* init map */
extern void map_init(void);

/* close map */
extern void map_close(void);

/* save map mode */
extern void map_save_mode(xmlNodePtr node);

/* load map mode */
extern void map_load_mode(xmlNodePtr node);


/* set the map palette */
extern void map_set_palette(void);

/* set map graphics */
extern void map_set_tiles(Archive * archive, int basictileindex,
                          int tileindex,
                          int largetileindex, int tiletypeindex);

/* set map palette */
extern void map_set_palette_resource(Archive * archive, int paletteindex);

/* set map description */
extern void map_set_map(Archive * archive, int id, int mapindex);


#ifdef TTT
/* enter building in TTT */
extern void map_enter_building(int id);

/* exit building int TTT */
extern void map_exit_building();

/* building enter coordinates */
extern void map_building_enter_coords(int id, int *x, int *y);

/* get building id */
extern int map_get_building(int map, int x, int y);
#endif

/* get map building_id */
extern int map_get_building_id(void);



/* set map command spots */
extern void map_set_spots(Archive * archive, int index);

/* load map */
extern void map_load(Uint8 * data);

/* save map */
extern void map_save(Uint8 * data);

/* display map - center at x, y */
extern void map_display(int x, int y);


/* map animate frame */
extern void map_animate_frame(void);

/* map set frame */
extern void map_set_frame(int frame);

/* map get frame */
extern int map_get_frame(void);

/* map is night */
extern int map_is_night(void);

/* adds character to the map */
extern void map_add_character(Character * character);

/* adds unique character to the map */
extern void map_unique_add_character(Character * character);

/* remove all characters */
extern void map_remove_all_characters(void);

/* removes character from the map */
extern void map_remove_character(int id);

/* updates character position */
extern void map_character_update(Character * character);




/* get terrain type */
extern int terrain_get(int x, int y);


/* is terrain passable? */
extern int terrain_free(int x, int y);

/* can character move to (x,y)? */
extern int map_can_move_to(Character * character, int x, int y);


/* returns a spot character is standing in */
extern CommandSpot *map_get_spot(Character * character);

/* get a spot by its number */
extern CommandSpot *map_get_spot_number(int index);


/* is character standing in the spot? */
extern int map_is_in_spot(Character * character, CommandSpot * spot, int *w,
                          int *h);


/* get npcs who stand near the given character */
extern int map_npc_stay_near(Character * character, int codes[10],
                             int in_party);


/* get map obejcts */
extern Uint8 *map_get_objects(void);

/* get map register */
extern int map_get_register(int index);

/* set register */
extern void map_set_register(int index, int value);


/* teleport character */
extern void map_character_teleport(Character * character, int rel, int x,
                                   int y, int dir, int map);

/* turn character */
extern void map_character_turn_to(int character_id, int x, int y);

/* move character */
extern int map_character_move(int character_id, int x, int y, int dir);


/* set map mode */
extern void map_set_mode(int mode, int param);

/* get map id */
extern int map_get_id(void);

/* set attacking character */
extern void map_attacking_character(int id);

/* return if a character at this position is in displayed area */
extern int map_is_displayed(int x, int y);

/* set climb mode */
extern void map_set_climb_mode(int mode);

/* set map light mode */
extern void map_set_light(int mode);

/* set dark map */
extern void map_set_dark(void);

/* set normal map */
extern void map_set_normal(void);

/* is map dark? */
extern int map_is_dark(void);

/* get map center */
extern void map_get_center(int *x, int *y);

#ifdef EXTENDED
/* add new python spot */
extern void map_add_pythonspot(int map, int x, int y, int w, int h);

/* get python spot */
extern CommandSpot *map_get_pythonspot(int id);
#endif



#endif /* _MAP_H */
