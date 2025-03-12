/****************************************************************************

    game.c
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


#include "lotr.h"
#include "character.h"
#include "cartoon.h"
#include "combat.h"
#include "game.h"
#include "graphics.h"
#include "gui.h"
#include "lotr_sdl.h"
#include "map.h"
#include "object.h"
#include "spot.h"
#include "sound.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>




Character *leader;
Character *game_original_leader;
Character *game_tmp_leader;
int game_must_dismiss_tmp_leader;

int saved_position_x = -1;
int saved_position_y = -1;
int saved_position_dir = -1;
int saved_position_map = -1;

Character *game_party[11];
int game_party_size;

/* game registers */
Uint8 game_registers[256];

/* actual map id */
int game_map_id = 0;

/* actual map description */
int game_map_desc = 0;
int game_map_text = 0;
int game_map_gr = 0;


int loaded_map_desc = -1;
int loaded_map_spots = -1;
int loaded_map_graphics = -1;


#ifndef TTT
/* map_desc, map_spots+map_texts, map_graphics */
int game_maps[][4] = { {0x7, 0x0, 0x2},
{0xf, 0x0, 0xa},
{0x107, 0x100, 0x2},
{0x10f, 0x100, 0xa},
{0x117, 0x110, 0x2},
{0x11f, 0x110, 0xa},
{0x127, 0x120, 0x122},
{0x12f, 0x120, 0xa},
{0x0, 0x130, 0},
{0x13f, 0x130, 0x13a},
{0x147, 0x140, 0x142},
{0x14f, 0x140, 0xa},
{0x157, 0x150, 0x122},
{0x15f, 0x150, 0x13a}
};
#else
int game_maps[][4] = { {0x7, 0x0, 0x2},
{0xf, 0x0, 0xa},
{0x107, 0x100, 0x102},
{0x10f, 0x100, 0x10a},
{0x117, 0x110, 0x112},
{0x11f, 0x110, 0x11a},
{0x127, 0x120, 0x122},
{0x12f, 0x120, 0xa},
{0x137, 0x130, 0x132},
{0x13f, 0x130, 0x13a},
{0x147, 0x140, 0x142},
{0x14f, 0x140, 0x14a},
{0x157, 0x150, 0x152},
{0x15f, 0x150, 0x15a},
{0x167, 0x160, 0x162},
{0x16f, 0x160, 0x16a}
};
#endif



Uint8 game_map_saves[8][256];
int game_map_saved[8];


/* spot commands are running */
int game_spot_running = 0;

/* game timer */
int game_timer = -1;


/* group silver */
int silver_pennies = 0;

/* if party can move */
int game_moving = 1;

/* if party follows leader */
int game_follow = 1;


/* game texts */
#define GAME_TEXTS_MAX 512
Uint8 *game_text_data = NULL;
char *game_texts[GAME_TEXTS_MAX];
int game_text_num;
char game_par_text[256];



/* actual spot */
CommandSpot *game_actual_spot;

#ifdef DEMO
int demo_state = 0;
#endif



/*
  parse game texts
*/

void
game_parse_texts(Archive *archive, int index)
{
    int size;
    int i;

    if (game_text_data)
        free(game_text_data);

    game_text_data = decompress_ndxarchive(archive, index, &size);

    i = 0;
    game_text_num = 0;
    while (i < size) {
        if ((Uint8)game_text_data[i] == 0xff)
            break;
        if (game_text_num == GAME_TEXTS_MAX) {
            fprintf(stderr, "lotr: too many game texts\n");
            exit(1);
        }
#ifdef DEBUG
        printf("%02x %s\n", game_text_num, game_text_data + i);
#endif

        game_texts[game_text_num++] = (char *)game_text_data + i;
        while (game_text_data[i])
            ++i;
        ++i;
    }

}






/*
  loads a map
*/

void
game_load_map(int map)
{
    Archive *archive;
    int i;
#ifdef DEMO
    int only_one_map = 1;
#else
    int only_one_map = 0;
#endif

#ifndef TTT
    if (map < 0 || map > 13 || !game_maps[map][0]
        || (only_one_map && map > 0))
#else
    if (map < 0 || map > 15 || !game_maps[map][0]
        || (only_one_map && map > 0))
#endif
    {
        fprintf(stderr, "lotr: not existing map index=%d\n", map);
        exit(1);
    }
#ifdef TTT
    map_exit_building();
#endif

    graphics_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    graphics_clear_screen();
    graphics_update_screen();
    lotr_reset_keyboard();

    if (game_maps[map][0] != loaded_map_desc) {
#ifndef DEMO
        if (game_maps[map][0] > 0xff)
            archive = archive_ndx_open("map1");
        else
            archive = archive_ndx_open("map0");
#else
        archive = archive_ndx_open("demo");
#endif
        map_set_map(archive, map, game_maps[map][0] % 0x100);
        archive_close(archive);
        loaded_map_desc = game_maps[map][0];
    }


    if (game_maps[map][1] != loaded_map_spots) {
#ifndef DEMO
        if (game_maps[map][1] > 0xff)
            archive = archive_ndx_open("map1");
        else
            archive = archive_ndx_open("map0");
#else
        archive = archive_ndx_open("demo");
#endif

        game_parse_texts(archive, game_maps[map][1] % 0x100 + 1);

        if (game_map_id >= 0) {
            map_save(game_map_saves[game_map_id / 2]);
            game_map_saved[game_map_id / 2] = 1;
        }
        map_set_spots(archive, game_maps[map][1] % 0x100);
        if (game_map_saved[map / 2])
            map_load(game_map_saves[map / 2]);

        archive_close(archive);
        loaded_map_spots = game_maps[map][1];
        game_actual_spot = NULL;
        game_spot_running = 0;
    }


    game_map_id = map;

    if (game_maps[map][2] != loaded_map_graphics) {
#ifndef DEMO
        if (game_maps[map][2] > 0xff)
            archive = archive_ndx_open("map1");
        else
            archive = archive_ndx_open("map0");
#else
        archive = archive_ndx_open("demo");
#endif
        map_set_tiles(archive, game_maps[map][2] % 0x100,
                      game_maps[map][2] % 0x100 + 1,
                      game_maps[map][2] % 0x100 + 2,
                      game_maps[map][2] % 0x100 + 4);
        map_set_palette_resource(archive, game_maps[map][2] % 0x100 + 3);
        archive_close(archive);
        loaded_map_graphics = game_maps[map][2];
    }

    character_fill_map(map, 0xff);

    for (i = 0; i < game_party_size; ++i)
        map_unique_add_character(game_party[i]);

    game_timer = -1;

#ifdef EXTENDED
    pythonspot_init_map(map);
#endif

    gui_clear();

}


/*
  starts a new game
*/

void
game_new(void)
{

    int i;

    play_music();

    for (i = 0; i < 8; ++i)
        game_map_saved[i] = 0;
    loaded_map_spots = loaded_map_desc = loaded_map_graphics = -1;
    game_map_id = -1;

#ifndef TTT
    game_load_map(0);
#else
    game_load_map(6);
#endif

#ifdef DEMO
    map_set_frame(0x400);
#else
    map_set_frame(0x200);
#endif


    game_party_size = 0;
    silver_pennies = 0;
    game_moving = 1;
    game_follow = 1;

    saved_position_x = -1;
    saved_position_y = -1;
    saved_position_dir = -1;
    saved_position_map = -1;

    for (i = 0; i < 256; ++i)
        game_registers[i] = 0;

#ifndef TTT
    /* Frodo is the leader */
    leader = character_get(0xa0);
#else
    leader = NULL;
    game_party_activate(1);
#endif

    game_recruit(leader, 1);

    game_original_leader = NULL;
    game_tmp_leader = NULL;

}







/*
  save game
*/

int
game_save(int n)
{

    char name[20];
    int i;


    Uint8 buf[10];

    xmlDocPtr doc;
    xmlNodePtr root;
    xmlNodePtr node;
    xmlNodePtr subnode;

    doc = xmlNewDoc((const xmlChar *)"1.0");
    xmlSetDocCompressMode(doc, 6);


    root = xmlNewNode(NULL, (const xmlChar *)"lotr_savegame");
    xmlDocSetRootElement(doc, root);

#ifndef TTT
    lotr_save_prop_int(root, "vol", 1);
#else
    lotr_save_prop_int(root, "vol", 2);
#endif


    node = xmlNewNode(NULL, (const xmlChar *)"characters");
    xmlAddChild(root, node);

    characters_save(node);


    node = xmlNewNode(NULL, (const xmlChar *)"map_saves");
    xmlAddChild(root, node);

    map_save(game_map_saves[game_map_id / 2]);
    game_map_saved[game_map_id / 2] = 1;

    for (i = 0; i < 8; ++i)
        if (game_map_saved[i]) {
            subnode = xmlNewNode(NULL, (const xmlChar *)"map");
            lotr_save_prop_int(subnode, "map_num", i);
            lotr_save_prop_field(subnode, "map_data", game_map_saves[i],
                                 0x100);
            xmlAddChild(node, subnode);
        }



    lotr_save_prop_int(root, "game_map_id", game_map_id);




    node = xmlNewNode(NULL, (const xmlChar *)"map");
    xmlAddChild(root, node);

    map_save_mode(node);



    lotr_save_prop_field(root, "game_registers", game_registers, 256);


    node = xmlNewNode(NULL, (const xmlChar *)"party");
    xmlAddChild(root, node);

    lotr_save_prop_int(node, "silver", silver_pennies);
    lotr_save_prop_int(node, "moving", game_moving);
    lotr_save_prop_int(node, "follow", game_follow);

    lotr_save_prop_int(node, "follow", game_follow);

    for (i = 0; i < game_party_size; ++i)
        buf[i] = game_party[i]->id;

    lotr_save_prop_field(node, "party", buf, game_party_size);

    lotr_save_prop_int(node, "leader", leader->id);

    node = xmlNewNode(NULL, (const xmlChar *)"saved_position");
    xmlAddChild(root, node);

    lotr_save_prop_int(node, "x", saved_position_x);
    lotr_save_prop_int(node, "y", saved_position_y);
    lotr_save_prop_int(node, "dir", saved_position_dir);
    lotr_save_prop_int(node, "map", saved_position_map);


#ifndef TTT
    snprintf(name, sizeof(name), "savegame.%d", n);
#else
    snprintf(name, sizeof(name), "savegame2.%d", n);
#endif

    xmlSaveFormatFileEnc(lotr_homedir_filename(name), doc, "ascii", 1);

    xmlFreeDoc(doc);

    return 1;
}


/*
  load game
*/

int
game_load(int n)
{

    char name[20];
    int i;
    int vol;

    Uint8 buf[10];

    xmlDocPtr doc;
    xmlNodePtr root;
    xmlNodePtr node;
    xmlNodePtr subnode;

#ifndef TTT
    snprintf(name, sizeof(name), "savegame.%d", n);
#else
    snprintf(name, sizeof(name), "savegame2.%d", n);
#endif

    doc = xmlParseFile(lotr_homedir_filename(name));
    if (doc == NULL)
        return 0;

    root = xmlDocGetRootElement(doc);
    if (root == NULL)
        return 0;

    vol = lotr_load_prop_int_default(root, "vol", 1);
#ifndef TTT
    if (vol != 1)
        return 0;
#else
    if (vol != 2)
        return 0;
#endif

    node = lotr_get_subnode(root, (const xmlChar *)"characters", 1);
    characters_load(node);


    node = lotr_get_subnode(root, (const xmlChar *)"map_saves", 1);

    for (i = 0; i < 8; ++i)
        game_map_saved[i] = 0;

    subnode = node->xmlChildrenNode;

    while (subnode != NULL) {
        if (xmlNodeIsText(subnode)) {
            subnode = subnode->next;
            continue;
        }
        i = lotr_load_prop_int(subnode, "map_num");
        game_map_saved[i] = 1;
        lotr_load_prop_field(subnode, "map_data", game_map_saves[i], 0x100);

        subnode = subnode->next;
    }



    loaded_map_spots = loaded_map_desc = loaded_map_graphics = -1;
    game_map_id = -1;

    game_load_map(lotr_load_prop_int(root, "game_map_id"));


    node = lotr_get_subnode(root, (const xmlChar *)"map", 1);
    map_load_mode(node);


    if (lotr_get_subnode(root, (const xmlChar *)"game_registers", 0))
        lotr_load_prop_field(root, "game_registers", game_registers, 256);
    else
        for (i = 0; i < 256; ++i)
            game_registers[i] = 0;


    node = lotr_get_subnode(root, (const xmlChar *)"party", 1);

    silver_pennies = lotr_load_prop_int(node, "silver");
    game_moving = lotr_load_prop_int(node, "moving");
    game_follow = lotr_load_prop_int(node, "follow");

    game_party_size = lotr_load_prop_field(node, "party", buf, 11);

    for (i = 0; i < game_party_size; ++i)
        game_party[i] = character_get(buf[i]);


    leader = character_get(lotr_load_prop_int(node, "leader"));

    node = lotr_get_subnode(root, (const xmlChar *)"saved_position", FALSE);

    if (node) {
        saved_position_x = lotr_load_prop_int(node, "x");
        saved_position_y = lotr_load_prop_int(node, "y");
        saved_position_dir = lotr_load_prop_int(node, "dir");
        saved_position_map = lotr_load_prop_int(node, "map");
    } else {
        saved_position_x = -1;
        saved_position_y = -1;
        saved_position_dir = -1;
        saved_position_map = -1;
    }


    xmlFreeDoc(doc);


    return 1;
}





#ifdef TTT
/*
  convert vol. I characters to vol. II
*/

void
game_convert(int game_id)
{
    char name[20];
    int vol;
    xmlDocPtr doc;
    xmlNodePtr root;
    xmlNodePtr node;
    Uint8 buf[10];
    int i;

    int party_size;

    snprintf(name, sizeof(name), "savegame.%d", game_id);

    doc = xmlParseFile(lotr_homedir_filename(name));
    if (doc == NULL)
        return;

    root = xmlDocGetRootElement(doc);
    if (root == NULL)
        return;

    vol = lotr_load_prop_int_default(root, "vol", 1);
    if (vol != 1)
        return;

    node = lotr_get_subnode(root, (const xmlChar *)"party", 1);
    /* give silver to Aragorn */
    character_get(0xa4)->silver = lotr_load_prop_int(node, "silver");

    party_size = lotr_load_prop_field(node, "party", buf, 11);

    node = lotr_get_subnode(root, (const xmlChar *)"characters", 1);

    character_convert(node, 0xa0);
    for (i = 0; i < party_size; ++i)
        if (buf[i] >= 0xa1 && buf[i] <= 0xa7 && buf[i] != 0xa5)
            character_convert(node, buf[i]);

    xmlFreeDoc(doc);


}
#endif







/*
  parse keyboard movement input
*/


void
game_leader_movement(void)
{

    int w, h;
    CommandSpot *spot;

    if (leader->action != CHARACTER_STAY)
        return;

#ifdef DEMO
    if (demo_state < 0x100)
        return;
#endif

#ifdef ENABLE_CHEATS
    if (lotr_key_shift() || lotr_key_ctrl())
#else
    if (0)
#endif
    {
        if (lotr_key_ctrl()) {
            if (lotr_key_left() && leader->x >= 4)
                leader->x -= 4;
            if (lotr_key_right() && leader->x < MAP_WIDTH * 16 * 4 - 20)
                leader->x += 4;
            if (lotr_key_up() && leader->y >= 4)
                leader->y -= 4;
            if (lotr_key_down() && leader->y < MAP_HEIGHT * 16 * 4 - 20)
                leader->y += 4;
            map_character_update(leader);
        } else {
            if (lotr_key_left() && leader->x >= 0x20)
                leader->x -= 0x20;
            if (lotr_key_right()
                && leader->x < MAP_WIDTH * 16 * 4 - 20 - 0x20)
                leader->x += 0x20;
            if (lotr_key_up() && leader->y >= 0x20)
                leader->y -= 0x20;
            if (lotr_key_down()
                && leader->y < MAP_HEIGHT * 16 * 4 - 20 - 0x20)
                leader->y += 0x20;
            map_character_update(leader);
        }
    } else {

        if (!map_is_in_spot(leader, game_actual_spot, &w, &h))
            game_actual_spot = NULL;

#ifndef DEMO
        spot = map_get_spot(leader);
#else
        spot = NULL;
#endif

        if (game_actual_spot != spot) {
            game_timer = -1;

            game_actual_spot = spot;

            if (gui_if_show_spots())
                spot_print(game_actual_spot);

            game_spot_running = spot_start(game_actual_spot);

        }

        if (lotr_key_left()
            && map_can_move_to(leader, leader->x / 4 - 1, leader->y / 4))
            character_move_left(leader);

        if (lotr_key_right()
            && map_can_move_to(leader, leader->x / 4 + 1, leader->y / 4))
            character_move_right(leader);

        if (lotr_key_up()
            && map_can_move_to(leader, leader->x / 4, leader->y / 4 - 1))
            character_move_up(leader);

        if (lotr_key_down()
            && map_can_move_to(leader, leader->x / 4, leader->y / 4 + 1))
            character_move_down(leader);

        if (leader->action != CHARACTER_STAY && game_actual_spot != NULL)
            spot_action(game_actual_spot, SPOT_ACTION_MOVE, leader->action,
                        0xff);


    }

}








/*
  all party charcaters follow the leader
*/


void
game_follow_leader(void)
{

    int i, j, try;
    int next_x, next_y;

    if (!game_follow)
        return;

    leader->next_x = 0;
    leader->next_y = 0;

    for (i = 0; i < game_party_size; ++i) {
        if (game_party[i] == leader)
            continue;

        if (abs(game_party[i]->x - leader->x) > 4 * 16 ||
            abs(game_party[i]->y - leader->y) > 4 * 8) {
            game_party[i]->x = leader->x;
            game_party[i]->y = leader->y;
            game_party[i]->direction = leader->direction;
            map_character_update(game_party[i]);
        }


        character_frame(game_party[i]);

        if (game_party[i]->action == CHARACTER_STAY &&
            (abs(game_party[i]->x - leader->x) > 8 ||
             abs(game_party[i]->y - leader->y) > 8)) {
            for (try = 0; try < 2; ++try) {
                if (game_party[i]->action != CHARACTER_STAY)
                    continue;
                if (try ^
                    (abs(game_party[i]->x - leader->x) >
                     abs(game_party[i]->y - leader->y))) {
                    if (game_party[i]->x < leader->x) {
                        if (map_can_move_to
                            (game_party[i], game_party[i]->x / 4 + 1,
                             game_party[i]->y / 4))
                            character_move_right(game_party[i]);
                    } else {
                        if (map_can_move_to
                            (game_party[i], game_party[i]->x / 4 - 1,
                             game_party[i]->y / 4))
                            character_move_left(game_party[i]);
                    }
                } else {
                    if (game_party[i]->y > leader->y) {
                        if (map_can_move_to
                            (game_party[i], game_party[i]->x / 4,
                             game_party[i]->y / 4 - 1))
                            character_move_up(game_party[i]);
                    } else {
                        if (map_can_move_to
                            (game_party[i], game_party[i]->x / 4,
                             game_party[i]->y / 4 + 1))
                            character_move_down(game_party[i]);
                    }
                }
            }                   /* for( try ) */
        }

        next_x = game_party[i]->x;
        next_y = game_party[i]->y;

        switch (game_party[i]->action) {
            case CHARACTER_LEFT:
                next_x -= 4;
                break;
            case CHARACTER_RIGHT:
                next_x += 4;
                break;
            case CHARACTER_UP:
                next_y -= 4;
                break;
            case CHARACTER_DOWN:
                next_y += 4;
                break;
        }


        if (next_x != game_party[i]->x || next_y != game_party[i]->y) {
            for (j = 0; j < i; ++j)
                if (next_x == game_party[j]->next_x
                    && next_y == game_party[j]->next_y)
                    break;

            if (j == i) {
                game_party[i]->next_x = next_x;
                game_party[i]->next_y = next_y;
            } else {
                game_party[i]->next_x = game_party[i]->x;
                game_party[i]->next_y = game_party[i]->y;
                game_party[i]->action = CHARACTER_STAY;
            }
        }

    }

}




#ifdef DEMO
void demo_frame(void);
#endif


/*
  proceed to the next game frame
*/

void
game_next_frame(void)
{

    if (combat_frame())
        goto end_next_frame;

    if (gui_frame())
        goto end_next_frame;

    if (game_actual_spot == NULL)
        game_spot_running = 0;

#ifdef DEMO
    if (demo_state < 0x100) {
        demo_frame();
        if (!gui_mode())
            game_draw_map();
        goto end_next_frame;
    }
#endif

    if (!game_spot_running) {

        if (game_timer > 0) {
            game_timer--;
            if (game_timer == 0 && game_actual_spot != NULL)
                spot_action(game_actual_spot, SPOT_ACTION_TIMER, 0xff, 0xff);
        }

        character_frame(leader);

#ifdef TTT
        if (leader->action == CHARACTER_STAY)
#ifdef ENABLE_CHEATS
            if (!lotr_key_shift() && !lotr_key_ctrl())
#endif
            {
                int terrain;
                int building;
                terrain = terrain_get(leader->x / 4 + 2, leader->y / 4 + 1);
                if (terrain != TERRAIN_DOWN &&
                    terrain != TERRAIN_UP && terrain != TERRAIN_STAIRS_DOWN)
                    terrain =
                        terrain_get(leader->x / 4 + 1, leader->y / 4 + 1);
                if (terrain != TERRAIN_DOWN && terrain != TERRAIN_UP
                    && terrain != TERRAIN_STAIRS_DOWN)
                    terrain = terrain_get(leader->x / 4, leader->y / 4 + 1);

                building = map_get_building_id();


                if (building == -1) {
                    if (terrain == TERRAIN_DOWN) {
                        building =
                            map_get_building(game_map_id, leader->x,
                                             leader->y);
                        if (building != -1) {
                            int x, y;
                            map_enter_building(building);
                            for (y = 0; y < BUILDING_HEIGHT * 4; ++y)
                                for (x = 0; x < BUILDING_WIDTH * 4; ++x)
                                    if (terrain_get(x, y) == TERRAIN_UP)
                                        goto exit_found;
exit_found:
                            if (terrain_free(x - 1, y) &&
                                terrain_free(x - 1, y + 1) &&
                                terrain_get(x, y + 1) == TERRAIN_UP)
                                game_leader_teleport(0, x * 4 - 8, y * 4 + 8,
                                                     0xff, 0xff);
                            else
                                game_leader_teleport(0, x * 4 + 12,
                                                     y * 4 - 8, 0xff, 0xff);
                        }
                    }
                } else {
                    if (terrain == TERRAIN_DOWN)
                        game_leader_teleport(1, 0, 32 * 4 * 4, 0xff, 0xff);

                    if (terrain == TERRAIN_STAIRS_DOWN && leader->y > 0x200)
                        game_leader_teleport(1, 0, -32 * 4 * 4, 0xff, 0xff);

                    if (terrain == TERRAIN_UP) {
                        int x, y;
                        map_exit_building();
                        map_building_enter_coords(building, &x, &y);
                        game_leader_teleport(0, x, y, 0xff, 0xff);
                    }
                }
            }
#endif

        if (game_moving) {
            game_leader_movement();
            game_follow_leader();
        }

    } else {                    /* if( !game_spot_running  ) */

        game_spot_running = spot_continue(game_actual_spot);
    }

    if (!gui_mode())
        game_draw_map();

end_next_frame:

    map_animate_frame();
    graphics_update_screen();
}









/*
  draws the map
*/

void
game_draw_map(void)
{
    map_display(leader->x * 2, leader->y * 2);
}












/*
  init the graphics
*/

void
game_init_graphics(void)
{
    graphics_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    graphics_clear_screen();

    gui_clear();

    gui_set_palette();
    map_set_palette();
    shapes_set_palette();

}











/*
  get leader character
*/

Character *
game_get_leader(void)
{
    return leader;
}









/*
  set leader character
*/

int
game_set_leader(Character *character)
{
    int i;

    for (i = 0; i < game_party_size; ++i)
        if (game_party[i] == character)
            break;

    if (i == game_party_size)
        return 0;

    character->x = leader->x;
    character->y = leader->y;
    character->direction = leader->direction;

    map_character_update(character);

    leader = character;

    return 1;

}


/*
  set how much siver pennies has the group
*/

void
game_set_silver(int amount)
{
    silver_pennies = amount;
}


/*
  get how much siver pennies has the group
*/

int
game_get_silver()
{
    return silver_pennies;
}


/*
  group pays silver
*/

int
game_pay_silver(int amount)
{
    if (amount > silver_pennies)
        return 0;
    silver_pennies -= amount;
    return 1;
}


/*
  group gets silver
*/

void
game_add_silver(int amount)
{
    silver_pennies += amount;
}




/*
  returns game text
*/

char *
game_get_text(int index)
{
    if (index < 0 || index >= game_text_num)
        return "<null>";
    return game_texts[index];
}


/*
  returns game paragraph text
*/

char *
game_get_paragraph_text(int index, int question)
{
    if (!PIXEL_PRECISE && question)
        snprintf(game_par_text, sizeof(game_par_text), "See paragraph number %d. (Y/N)", index);
    else
        snprintf(game_par_text, sizeof(game_par_text), "See paragraph number %d.", index);
    return game_par_text;
}




/*
  is character in the party?
*/

int
game_in_party(Character *character)
{
    int i;
    for (i = 0; i < game_party_size; ++i)
        if (game_party[i] == character)
            return 1;
    return 0;
}


/*
  get the game party
*/

int
game_get_party(int codes[10])
{
    int i, s;
    s = game_party_size;
    if (s > 10)
        s = 10;

    for (i = 0; i < s; ++i)
        codes[i] = game_party[i]->id;

    return s;
}



/*
  get the game party characters
*/

int
game_get_party_characters(Character *party[11])
{
    int i;

    for (i = 0; i < game_party_size; ++i)
        party[i] = game_party[i];

    return game_party_size;
}


/*
  set game party characters
*/

void
game_set_party_characters(Character *party[11], int size)
{
    int i;
    int has_leader = 0;

    game_party_size = 0;
    if (leader == NULL)
        leader = party[0];

    if (size == 0) {
        fprintf(stderr, "lotr: empty party - no characters to play\n");
        exit(1);
    }

    for (i = 0; i < size; ++i) {
        game_recruit(party[i], 1);
        if (party[i] == leader)
            has_leader = 1;
    }

    if (!has_leader)
        leader = party[0];

}



/*
  activate party
*/

void
game_party_activate(int id)
{
    character_set_party(id);
    game_load_map(leader->map);
}



/*
  recruit character to the party
*/

int
game_recruit(Character *character, int force)
{

    int i;

    if (game_party_size >= 10)
        return 0;
    if (game_in_party(character))
        return 0;
    if (!force && !character->willing_join)
        return 0;
    character->willing_join = 1;
    game_party[game_party_size++] = character;

    if (!character->actived && !character->converted) {
        for (i = 0; i < character->items_num; ++i)
            if (object_is_weapon(character->items[i])) {
                character_use(character, i);
                break;
            }

        for (i = 0; i < character->items_num; ++i)
            if (object_is_armour(character->items[i])) {
                character_use(character, i);
                break;
            }

        for (i = 0; i < character->items_num; ++i)
            if (object_is_shield(character->items[i])) {
                character_use(character, i);
                break;
            }
    }

    silver_pennies += character->silver;
    character->silver = 0;
    character->actived = 1;

    character->action = CHARACTER_STAY;
#ifdef TTT
    if (character->party_id != leader->party_id)
        fprintf(stderr, "lotr: WARNING recruiting to a wrong party?\n");
    character->party_id = leader->party_id;
#endif

    game_check_light();
    return 1;
}











/*
  dismiss character
*/

int
game_dismiss(Character *character)
{
    int i;

    for (i = 0; i < game_party_size; ++i)
        if (game_party[i] == character)
            break;

    if (i == game_party_size)
        return 0;

    character->action = CHARACTER_STAY;
    character->party_id = 0xff;

    for (; i + 1 < game_party_size; ++i)
        game_party[i] = game_party[i + 1];

    game_party_size--;

    if (character == leader) {
        leader = game_party[0];
        if (game_party_size == 0) {
            fprintf(stderr, "lotr: error: empty party\n");
            exit(1);
        }
    }

    game_check_light();

    return 1;
}







/*
  teleport leader
*/

int
game_leader_teleport(int rel, int x, int y, int dir, int map)
{

    int i;
    int result = 0;

    leader->action = CHARACTER_STAY;

    if (map != 0xff && map != game_map_id) {
        game_load_map(map);
        for (i = 0; i < game_party_size; ++i)
            map_unique_add_character(game_party[i]);
        result = 1;
    }


    map_character_teleport(leader, rel, x, y, dir, map);


    for (i = 0; i < game_party_size; ++i) {
        game_party[i]->x = leader->x;
        game_party[i]->y = leader->y;
        game_party[i]->direction = leader->direction;
        map_character_update(game_party[i]);
    }

    return result;

}





/*
  save current position for further reference
 */
void
game_save_position()
{
    saved_position_x = leader->x;
    saved_position_y = leader->y;
    saved_position_dir = leader->direction;
    saved_position_map = game_map_id;
}



/*
  teleport to the saved position
  return true if map was changed
 */
int
game_load_position()
{
    if (saved_position_x < 0) {
        fprintf(stderr, "lotr: Trying to load a saved position whiile none was saved\n");
        return FALSE;
    }

    return game_leader_teleport(FALSE, saved_position_x, saved_position_y, saved_position_dir, saved_position_map);
}



/*
  gets actual game spot
*/

CommandSpot *
game_get_actual_spot(void)
{
    return game_actual_spot;
}




/*
  continue command spot
*/

void
game_continue_spot(void)
{
    if (game_actual_spot != NULL)
        game_spot_running = 1;
}










/*
  get game register
*/

int
game_get_register(int index)
{

    if (index < 0 || index >= 256) {
        fprintf(stderr, "lotr: wrong game register (index=%x)\n", index);
        exit(1);
    }

    return game_registers[index];
}









/*
  set game register
*/

void
game_set_register(int index, int value)
{
    /* check that we are in good range */
    game_get_register(index);

    game_registers[index] = value;

}


/*
  set game timer
*/

void
game_set_timer(int ticks)
{
    game_timer = ticks;
}


/*
  set game tmp leader
 */

void
game_set_tmp_leader(Character *tmp_leader)
{
    if (game_tmp_leader) {
        fprintf(stderr, "lotr: game tmp leader already active\n");
        exit(1);
    }

    game_must_dismiss_tmp_leader = 0;

    if (!game_in_party(tmp_leader)) {
        game_must_dismiss_tmp_leader = 1;
        /* we have one extra slot for tmp leader so no check needed */
        game_party[game_party_size++] = tmp_leader;
        tmp_leader->actived = 1;
        tmp_leader->action = CHARACTER_STAY;
    }

    game_original_leader = leader;
    leader = game_tmp_leader = tmp_leader;

}


/*
  dismiss game tmp leader
 */

void
game_dismiss_tmp_leader(void)
{
    if (!game_tmp_leader) {
        fprintf(stderr, "lotr: game tmp leader not active\n");
        exit(1);
    }

    if (game_must_dismiss_tmp_leader)
        game_dismiss(game_tmp_leader);

    leader = game_original_leader;
    game_tmp_leader = NULL;

}



/*
  check if we have torch lit
*/

void
game_check_light(void)
{
    int i, j;

    for (i = 0; i < game_party_size; ++i)
        for (j = 0; j < game_party[i]->items_num; ++j)
            if (game_party[i]->items[j] == OBJECT_TORCH &&
                game_party[i]->item_used[j]) {
                map_set_light(1);
                return;
            }

    map_set_light(0);
}




/*
  set if characters can move
*/

void
game_set_moving(int moving)
{
    game_moving = moving;
}

/*
  get if characters can move
*/

int
game_get_moving(void)
{
    return game_moving;
}


/*
  set if party is following leader
*/

void
game_set_follow(int follow)
{
    game_follow = follow;
}



#ifdef DEMO
void
demo_frame(void)
{

    Character *character;

    switch (demo_state) {
        case 0:
            gui_message(game_texts[0], 0);
            ++demo_state;
            break;

        case 1:
            if (!map_character_move(leader->id, 0x21c, 0xc8, 0xff))
                ++demo_state;
            break;

        case 2:
            gui_message
                ("You remember what Gandalf said: \"But I do not think you need go alone. Not if you know of anyone you can trust.\"",
                 0);
            ++demo_state;
            break;

        case 3:
            game_recruit(character_get(0xa1), 1);
            game_recruit(character_get(0xa2), 1);
            ++demo_state;
            break;

        case 4:
            game_follow_leader();
            if (character_get(0xa2)->action == CHARACTER_STAY)
                ++demo_state;
            break;

        case 5:
            if (!map_character_move(leader->id, 0x1a0, 0xd8, 0xff))
                ++demo_state;
            break;

        case 6:
            ++demo_state;
            cartoon_play("democrt1");
            graphics_clear_screen();
            gui_set_palette();
            gui_clear();
            map_set_palette();
            shapes_set_palette();
            lotr_reset_keyboard();
            break;

        case 7:
            if (!map_character_move(leader->id, 0xdc, 0xd8, 0xff))
                ++demo_state;
            break;

        case 8:
            gui_message
                ("You said you are leaving to Buckland. But the truth is you are leaving Shire.",
                 0);
            ++demo_state;
            break;

        case 9:
            if (!map_character_move(leader->id, 0xdc, 0x170, 0xff))
                ++demo_state;
            break;

        case 10:
            gui_message
                ("Bag End, widely renowned as the most comfortable Hole in Hobbiton, was until today home of the Baggins Family. Now you have sold it to your relatives - Sackville-Baggins.",
                 0);
            ++demo_state;
            break;

        case 11:
            gui_message(game_texts[1], 0);
            ++demo_state;
            break;

        case 12:
            gui_message("You do it only reluctantly.", 0);
            ++demo_state;
            break;

        case 13:
            gui_message(game_texts[2], 0);
            ++demo_state;
            break;

        case 14:
            if (!map_character_move(0xb0, 0xffff, 0x120, 0xff))
                demo_state = 32;
            break;

        case 32:
            game_leader_teleport(0, 0x494, 0x914, 1, 0xff);
            ++demo_state;
            break;

        case 33:
            if (!map_character_move(leader->id, 0x4b8, 0xffff, 0xff))
                ++demo_state;
            break;

        case 34:
            character = character_get(0x95);
            character->x = leader->x + 4 * 4;
            character->y = leader->y - 2 * 4;
            character->map = 0;
            character->direction = 3;

            map_unique_add_character(character);

            gui_message("Black riders will chase you.", 0);
            ++demo_state;
            break;

        case 35:
            character = character_get(0xa4);
            character->x = leader->x - 2 * 4;
            character->y = leader->y - 2 * 4;
            character->map = 0;
            character->direction = 2;

            map_add_character(character);

            gui_message("But you will meet unexpected friends.", 0);
            ++demo_state;
            break;

        case 36:
            if (!map_character_move(0x95, 0x510, 0xffff, 0xff))
                demo_state = 64;
            break;


        case 64:
            game_leader_teleport(0, 0x95c, 0xb58, 1, 0xff);
            ++demo_state;
            break;

        case 65:
            if (!map_character_move(leader->id, 0x97c, 0xffff, 0xff))
                ++demo_state;
            break;

        case 66:
            gui_message
                ("At the Buckland Ferry you meet Merry. He joins your Quest.",
                 0);
            ++demo_state;
            break;

        case 67:
            game_recruit(character_get(0xa3), 1);
            demo_state = 96;
            break;


        case 96:
            game_leader_teleport(0, 0xc34, 0xaa4, 1, 0xff);
            ++demo_state;
            break;

        case 97:
            if (!map_character_move(leader->id, 0xc88, 0xffff, 0xff))
                ++demo_state;
            break;

        case 98:
            gui_message("After many perils...", 0);
            ++demo_state;
            break;

        case 99:
            cartoon_play("democrt2");
            graphics_clear_screen();
            gui_set_palette();
            gui_clear();
            map_set_palette();
            shapes_set_palette();
            demo_state = 128;
            lotr_reset_keyboard();
            break;

        case 128:
            game_leader_teleport(0, 0x21c, 0xb0, 2, 0xff);
            ++demo_state;
            break;

        case 129:
            gui_message
                ("Here the official demo ends. You can explore Shire on your own. Many things are not working - this was originally non-playable demo.",
                 0);
            ++demo_state;
            break;


        default:
            demo_state = 0x100;
            break;
    }

}
#endif
