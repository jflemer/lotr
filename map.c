/****************************************************************************

    map.c
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


#include "lord.h"
#include "archive.h"
#include "character.h"
#include "combat.h"
#include "game.h"
#include "map.h"
#include "spot.h"
#include "gui.h"
#include "graphics.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>




#define TILESIZE         8
#define BASICTILENUM  1280
#define TILENUM       1280
#define LARGETILENUM   768

/* map id */
int map_id = 0;
int building_id = -1;

/* map frame */
int map_frame;

/* animation frame */
int map_anim_frame;

/* map fade in % */
int map_fade;

/* map dimensions */
int map_width, map_height;

/* map center */
int map_last_x, map_last_y;

/* climb mode */
int map_climb_mode = 0;


/* buildings in TTT */
#ifdef TTT
#include "building_enters.h"

#define BUILDINGS_NUM 104

int buildings[BUILDINGS_NUM][4];

#endif

/* map tiles */
Pixmap *basictiles[BASICTILENUM];
int tiles[TILENUM][4][4];
int largetiles[LARGETILENUM][4][4];

int tileterrain[TILENUM];


/* map palette */
Palette *map_palette;


/* map mode (normal, dark, fog) */
int map_mode;
int map_mode_param;
int map_light_mode;             /* 0 not lighted, 1 torch, 2 illuminated */

/* the map description */
int map_desc[MAP_WIDTH][MAP_HEIGHT];

/* the building description */
int building_desc[BUILDING_WIDTH][BUILDING_HEIGHT];


/* characters */
#define MAX_CHARACTERS 256

#define MAP_CHAR_X   0
#define MAP_CHAR_Y   1
#define MAP_CHAR_DIR 2
#define MAP_CHAR_ID  3
int map_characters_num;
int map_characters[MAX_CHARACTERS][4];


#define MAX_DISPLAYED_CHARACTERS 32
int map_num_disp_char;
int map_displayed_characters[MAX_DISPLAYED_CHARACTERS * 5];


/* map command spots */
int map_num_spots;
CommandSpot *map_spots[MAX_COMMAND_SPOTS];
#ifdef EXTENDED
int map_num_pythonspots;
CommandSpot *map_pythonspots[MAX_PYTHON_SPOTS];
#endif

/* map registers */
Uint8 map_registers[16];
Uint8 map_objects[128 - 16];


#define DAY_FADE 100
#if PIXELPRECISE
#define NIGHT_FADE 50
#else
#define NIGHT_FADE 40
#endif
#define MAP_FADE_STEP 2



/*
  init map
*/

void
map_init(void)
{

#ifdef TTT
    Archive *archive;
    int i, j;

    archive = ndxarchiveopen("building");

    if (archive->size != BUILDINGS_NUM) {
        fprintf(stderr, "lord: expecting %d buildings (%d found)\n",
                BUILDINGS_NUM, archive->size);
        exit(1);
    }

    for (i = 0, j = 0; i < BUILDINGS_NUM; ++i)
        if (archivedatasize(archive, i)) {
            buildings[i][0] = j;
            buildings[i][1] = building_enters[j][0];
            buildings[i][2] = building_enters[j][1];
            buildings[i][3] = building_enters[j][2];
            ++j;
        }
        else {
            buildings[i][0] = -1;
        }

    archiveclose(archive);

    building_id = -1;

#endif

    bzero(basictiles, sizeof(basictiles));
    bzero(tiles, sizeof(tiles));
    bzero(largetiles, sizeof(largetiles));
    bzero(tileterrain, sizeof(tileterrain));
    map_palette = 0;
    map_characters_num = 0;
    map_num_spots = 0;
#ifdef EXTENDED
    map_num_pythonspots = 0;
#endif
    map_frame = 0;
    map_anim_frame = 0;

}




/*
  load map
*/

void
map_load(Uint8 * data)
{
    int i;
    for (i = 0; i < map_num_spots; ++i)
        map_spots[i]->flag = data[i];

    memcpy(map_registers, data + 0x80, 16);
    memcpy(map_objects, data + 0x80 + 16, 128 - 16);

}

/*
  save map
*/

void
map_save(Uint8 * data)
{
    int i;
    for (i = 0; i < map_num_spots; ++i)
        data[i] = map_spots[i]->flag;

    for (; i < 0x80; ++i)
        data[i] = 0xff;

    memcpy(data + 0x80, map_registers, 16);
    memcpy(data + 0x80 + 16, map_objects, 128 - 16);

}



/*
  close map
*/

void
map_close(void)
{
    int i;

    for (i = 0; i < BASICTILENUM; ++i)
        if (basictiles[i])
            pixmap_free(basictiles[i]);

    if (map_palette)
        free(map_palette);

    for (i = 0; i < map_num_spots; ++i)
        spot_free(map_spots[i]);
#ifdef EXTENDED
    for (i = 0; i < map_num_pythonspots; ++i)
        spot_free(map_pythonspots[i]);
#endif
    map_num_spots = 0;

}





/*
  save map mode
*/

void
map_save_mode(xmlNodePtr node)
{
#ifdef TTT
    lord_save_prop_int(node, "building", building_id);
#endif
    lord_save_prop_int(node, "frame", map_frame);
    lord_save_prop_int(node, "mode", map_mode);
    lord_save_prop_int(node, "mode_param", map_mode_param);
    lord_save_prop_int(node, "light_mode", map_light_mode);
}


/*
  load map mode
*/

void
map_load_mode(xmlNodePtr node)
{
#ifdef TTT
    int building;
    building = lord_load_prop_int_default(node, "building", -1);
    if (building == -1)
        map_exit_building();
    else
        map_enter_building(building);
#endif
    map_set_frame(lord_load_prop_int(node, "frame"));
    map_mode = lord_load_prop_int(node, "mode");
    map_mode_param = lord_load_prop_int(node, "mode_param");
    map_light_mode = lord_load_prop_int(node, "light_mode");
}




/*
  set map fade
*/


void
map_set_fade(int fade)
{
    if (fade < NIGHT_FADE)
        fade = NIGHT_FADE;
    if (fade > DAY_FADE)
        fade = DAY_FADE;
    map_fade = fade;

    FadePalette(map_fade, 0x00, 0x40);
}




/*
  set map graphics
*/

void
map_set_tiles(Archive * archive, int basictileindex,
              int tileindex, int largetileindex, int tiletypeindex)
{
    Uint8 *data;
    int size;
    int i, k, l;

    data = ndxdecompressarchive(archive, basictileindex, &size);
    if (size != BASICTILENUM * TILESIZE * TILESIZE) {
        fprintf(stderr, "lord: basic tiles data corrupted\n");
        exit(1);
    }


    for (i = 0; i < BASICTILENUM; ++i) {
        if (!basictiles[i])
            basictiles[i] = pixmap_new(TILESIZE, TILESIZE);
        memcpy(basictiles[i]->data, data + i * TILESIZE * TILESIZE,
               TILESIZE * TILESIZE);
    }

    free(data);


    data = ndxdecompressarchive(archive, tileindex, &size);
    if (size != TILENUM * 2 * 16) {
        fprintf(stderr, "lord: tiles data corrupted\n");
        exit(1);
    }

    for (i = 0; i < TILENUM; ++i)
        for (l = 0; l < 4; ++l)
            for (k = 0; k < 4; ++k)
                tiles[i][k][l] =
                    data[(i * 16 + l * 4 + k) * 2] +
                    0x100 * data[(i * 16 + l * 4 + k) * 2 + 1];


    free(data);

    data = ndxdecompressarchive(archive, largetileindex, &size);
    if (size != LARGETILENUM * 2 * 16) {
        fprintf(stderr, "lord: large tiles data corrupted\n");
        exit(1);
    }

    for (i = 0; i < LARGETILENUM; ++i)
        for (l = 0; l < 4; ++l)
            for (k = 0; k < 4; ++k)
                largetiles[i][k][l] =
                    data[(i * 16 + l * 4 + k) * 2] +
                    0x100 * data[(i * 16 + l * 4 + k) * 2 + 1];

    free(data);

    data = ndxdecompressarchive(archive, tiletypeindex, &size);
    if (size != BASICTILENUM) {
        fprintf(stderr, "lord: tile terrain data corrupted\n");
        exit(1);
    }

    for (i = 0; i < BASICTILENUM; ++i) {
        tileterrain[i] = data[i];
        if (tileterrain[i] < 0 || tileterrain[i] > 15 ||
            (tileterrain[i] > 8 && tileterrain[i] < 10)) {
            fprintf(stderr, "lord: terrain type %d unknown\n",
                    tileterrain[i]);
#ifndef TTT
            exit(1);
#endif
        }
    }

    free(data);

}





/* set map palette */

void
map_set_palette_resource(Archive * archive, int paletteindex)
{

    int size;

    if (map_palette)
        free(map_palette);

    map_palette =
        (Palette *) ndxdecompressarchive(archive, paletteindex, &size);

    if (size != sizeof(Palette)) {
        fprintf(stderr, "lord: map palette data corrupted\n");
        exit(1);
    }

    map_set_palette();

}



/*
  set the map palette
*/

void
map_set_palette(void)
{
    /* 0x00 -- 0x40 are tile colors */
    SetPalette(map_palette, 0x00, 0x40);

    if (map_id % 2 == 0 && map_is_night())
        map_set_fade(NIGHT_FADE);
    else
        map_set_fade(DAY_FADE);
}




/*
  set map description
*/

void
map_set_map(Archive * archive, int id, int mapindex)
{
    Uint8 *data;
    int size;
    int k, l;

    map_id = id;
#ifdef TTT
    if (building_id != -1)
        map_exit_building();
#endif

    data = ndxdecompressarchive(archive, mapindex, &size);
    if (size != MAP_WIDTH * MAP_HEIGHT * 2) {
        fprintf(stderr, "lord: map data corrupted size=%d\n", size);
        exit(1);
    }

    for (l = 0; l < MAP_HEIGHT; ++l)
        for (k = 0; k < MAP_WIDTH; ++k)
            map_desc[k][l] =
                data[(l * MAP_WIDTH + k) * 2] +
                0x100 * data[(l * MAP_WIDTH + k) * 2 + 1];

    free(data);

    map_num_disp_char = 0;

    map_mode = MAP_NORMAL;
    map_climb_mode = 0;
    map_light_mode = 0;
}




#ifdef TTT

Palette *building_tmp_pal;

/*
  enter building in TTT
*/

void
map_enter_building(int id)
{

    Archive *building_archive;
    Archive *building_pal;
    Uint8 *data;
    int size;
    int k, l;

    if (id == building_id)
        return;
    if (building_id != -1)
        map_exit_building();
    if (buildings[id][0] == -1)
        return;

    building_archive = ndxarchiveopen("building");

    /* load the right map for a given building */
    if (buildings[id][1] != map_id)
        game_leader_teleport(1, 0, 0, 0xff, buildings[id][1]);

    building_pal = ndxarchiveopen("buildpal");

    building_tmp_pal = map_palette;
    map_palette = NULL;
    map_set_palette_resource(building_pal, id);



    data = ndxdecompressarchive(building_archive, id, &size);
    if (size != BUILDING_WIDTH * BUILDING_HEIGHT * 2) {
        fprintf(stderr, "lord: building data corrupted size=%d\n", size);
        exit(1);
    }

    for (l = 0; l < BUILDING_HEIGHT; ++l)
        for (k = 0; k < BUILDING_WIDTH; ++k)
            building_desc[k][l] =
                data[(l * BUILDING_WIDTH + k) * 2] +
                0x100 * data[(l * BUILDING_WIDTH + k) * 2 + 1];

    free(data);


    archiveclose(building_archive);
    archiveclose(building_pal);

    building_id = id;

    map_characters_num = 0;
    character_fill_map(map_id, buildings[id][0]);

}


/*
  exit building int TTT
*/

void
map_exit_building()
{
    if (building_id == -1)
        return;
    free(map_palette);
    map_palette = building_tmp_pal;
    map_set_palette();
    building_id = -1;
}



/*
  get building id
*/

int
map_get_building(int map, int x, int y)
{
    int i;
    int result = -1;
    int d = 1000;
    int dxx, dyy;

    for (i = 0; i < 13; ++i)
        if (buildings[map * 13 / 2 + i][0] != -1) {
            dxx = abs(x - buildings[map * 13 / 2 + i][2]);
            dyy = abs(y - buildings[map * 13 / 2 + i][3]);
            if (dxx < 0x20 && dyy < 0x20 && dxx + dyy < d) {
                d = dxx + dyy;
                result = map * 13 / 2 + i;
            }
        }

    return result;

}



/*
  building enter coordinates
*/

void
map_building_enter_coords(int id, int *x, int *y)
{
    *x = buildings[id][2];
    *y = buildings[id][3];
}

#endif



/*
  set map command spots
*/

void
map_set_spots(Archive * archive, int index)
{
    int i;
    int size;
    Uint8 *data;

    for (i = 0; i < map_num_spots; ++i)
        spot_free(map_spots[i]);
    map_num_spots = 0;

    data = ndxdecompressarchive(archive, index, &size);

    if (size < 0x100) {
        fprintf(stderr, "lord: corrupted game description data\n");
        exit(1);
    }

    spot_parse(data, size, map_spots, &map_num_spots);

    memcpy(map_registers, data + 0x80, 0x10);
    memcpy(map_objects, data + 0x90, 0x70);

    free(data);

}




/*
  draws tile
 */

inline void
map_draw_tile(int tile, int x, int y)
{
    int i, j;

    for (j = 0; j < 4; ++j)
        for (i = 0; i < 4; ++i)
            pixmap_draw(basictiles[tiles[tile][i][j]],
                        x + i * TILESIZE, y + j * TILESIZE);
}



/*
  draws large tile
 */

inline void
map_draw_largetile(int tile, int x, int y)
{
    int i, j;
    int tilesize = 4 * TILESIZE;

    for (j = 0; j < 4; ++j)
        for (i = 0; i < 4; ++i)
            map_draw_tile(largetiles[tile][i][j], x + i * tilesize,
                          y + j * tilesize);
}





int
map_compar_char_pos(const void *a, const void *b)
{
    const int *aa = a;
    const int *bb = b;

    if (aa[1] < bb[1])
        return -1;
    if (aa[1] > bb[1])
        return 1;

    if (aa[0] < bb[0])
        return -1;
    if (aa[0] > bb[0])
        return 1;

    return 0;
}



/*
  draw command spot - map centered in x, y
 */

void
map_draw_spot(CommandSpot * spot, int x, int y, int color)
{
    int j;
    int sx, sy, sh, sw;
    int xx, yy;

    j = 5;
    while (j + 6 < spot->headersize) {
        sx = spot->data[j] + 0x100 * spot->data[j + 1];
        sy = spot->data[j + 2] + 0x100 * spot->data[j + 3];
        sw = spot->data[j + 4];
        sh = spot->data[j + 5];

#ifdef TTT
        if (spot->building != -1)
            sy += spot->floor * 32 * 4 * TILESIZE / 2;
#endif

        xx = 2 * sx - (x - (map_width + 1) / 2) + TILESIZE;
        yy = 2 * sy - (y - (map_height + 1) / 2) + TILESIZE;

        if (spot->flag)
            draw_rectangle(color, xx, yy, xx + sw * 2 + 1, yy + sh * 2 + 1);
        else
            draw_rectangle(0x42, xx, yy, xx + sw * 2 + 1, yy + sh * 2 + 1);

        draw_rectangle(0, xx + 1, yy + 1, xx + sw * 2, yy + sh * 2);
        j += 6;
    }

}


/*
  display map - center at x, y
*/



void
map_display(int x, int y)
{

    int largetilesize = 4 * 4 * TILESIZE;
    int startx, endx;
    int starty, endy;

    int i, j;
    int xx, yy;


    gui_set_map_area(&map_width, &map_height);


    if (combat_get_mode()) {
        x = map_last_x;
        y = map_last_y;
    }
    else {
        if (x < (map_width + 1) / 2)
            x = (map_width + 1) / 2;
        if (y < (map_height + 1) / 2)
            y = (map_height + 1) / 2;
#ifdef TTT
        if (building_id != -1 && x > 32 * 4 * TILESIZE - (map_width + 1) / 2)
            x = 32 * 4 * TILESIZE - (map_width + 1) / 2;
#endif
        map_last_x = x;
        map_last_y = y;
    }

    /* calculate which tiles to draw */

#ifdef TTT
    if (building_id != -1)
        largetilesize = 4 * TILESIZE;
#endif

    startx = (x - (map_width + 1) / 2) / largetilesize;
    endx = (x + (map_width + 1) / 2) / largetilesize;
    endx += 1;

#ifndef TTT
    if (endx > MAP_WIDTH)
        endx = MAP_WIDTH;
#else
    if (building_id == -1 && endx > MAP_WIDTH)
        endx = MAP_WIDTH;
    if (building_id != -1 && endx > BUILDING_WIDTH)
        endx = BUILDING_WIDTH;
#endif

    starty = (y - (map_height + 1) / 2) / largetilesize;
    endy = (y + (map_height + 1) / 2) / largetilesize;
    endy += 1;


#ifndef TTT
    if (endy > MAP_HEIGHT)
        endy = MAP_HEIGHT;
#else
    if (building_id == -1 && endy > MAP_HEIGHT)
        endy = MAP_HEIGHT;
    if (building_id != -1 && endy > BUILDING_HEIGHT)
        endy = BUILDING_HEIGHT;
#endif


    /*
       draw map
     */

#ifdef TTT
    if (building_id == -1)
#endif
        if (map_mode == MAP_NORMAL || map_mode == MAP_LIGHTED)
            for (j = starty; j < endy; ++j)
                for (i = startx; i < endx; ++i)
                    map_draw_largetile(map_desc[i][j],
                                       i * largetilesize - (x -
                                                            (map_width +
                                                             1) / 2),
                                       j * largetilesize - (y -
                                                            (map_height +
                                                             1) / 2));


#ifdef TTT
    if (building_id != -1
        && (map_mode == MAP_NORMAL || map_mode == MAP_LIGHTED))
        for (j = starty; j < endy; ++j)
            for (i = startx; i < endx; ++i)
                map_draw_tile(building_desc[i][j],
                              i * largetilesize - (x - (map_width + 1) / 2),
                              j * largetilesize - (y - (map_height + 1) / 2));
#endif


    if (map_mode == MAP_DARK)
        ClearScreen();

    if (map_mode == MAP_FOG)
        SetScreen(map_mode_param);


    /*
       draw spots
     */

    if (gui_if_show_spots()) {
        /* draw normal spots */
        for (i = 0; i < map_num_spots; ++i) {
            if (map_spots[i]->map == map_id) {
                CommandSpot *spot = map_spots[i];
#ifdef TTT
                if (spot->building != building_id)
                    continue;
#endif
                map_draw_spot(spot, x, y, 0x41);
            }
        }
        /* draw python spots */
#ifdef EXTENDED
        for (i = 0; i < map_num_pythonspots; ++i) {
            if (map_pythonspots[i]->map == map_id) {
                CommandSpot *spot = map_pythonspots[i];
#ifdef TTT
                if (spot->building != building_id)
                    continue;
#endif
                map_draw_spot(spot, x, y, 0x48);
            }
        }
#endif
    }



    /*
       draw characters
     */


    map_num_disp_char = 0;
    for (i = 0; i < map_characters_num; ++i)
        if (map_characters[i][MAP_CHAR_ID]) {
            xx = 2 * map_characters[i][MAP_CHAR_X] - (x -
                                                      (map_width + 1) / 2) +
                3 * TILESIZE;
            yy = 2 * map_characters[i][MAP_CHAR_Y] - (y -
                                                      (map_height + 1) / 2) +
                2 * TILESIZE;

            if (xx > -100 && yy >= -100 && xx < map_width + 100
                && yy < map_height + 100) {
                if (map_num_disp_char == MAX_DISPLAYED_CHARACTERS) {
                    fprintf(stderr, "Too many chracters displayed at once\n");
                    exit(1);
                }

                map_displayed_characters[map_num_disp_char * 5] = xx;
                map_displayed_characters[map_num_disp_char * 5 + 1] = yy;
                map_displayed_characters[map_num_disp_char * 5 + 2] =
                    map_characters[i][MAP_CHAR_ID];
                map_displayed_characters[map_num_disp_char * 5 + 3] =
                    map_characters[i][MAP_CHAR_DIR];
                map_displayed_characters[map_num_disp_char * 5 + 4] = i;
                ++map_num_disp_char;
            }
        }

    qsort(map_displayed_characters, map_num_disp_char, sizeof(int) * 5,
          map_compar_char_pos);

    for (i = 0; i < map_num_disp_char; ++i) {
        character_draw(map_displayed_characters[i * 5 + 2],
                       map_displayed_characters[i * 5 + 0],
                       map_displayed_characters[i * 5 + 1],
                       map_displayed_characters[i * 5 + 3]);
    }

    gui_set_full_window();

}


/*
  map set frame
*/

void
map_set_frame(int frame)
{
    map_frame = frame;
    if (map_id % 2 == 0 && map_is_night())
        map_set_fade(NIGHT_FADE);
    else
        map_set_fade(DAY_FADE);
}



/*
  map get frame
*/

int
map_get_frame(void)
{
    return map_frame;
}



/*
  map is night
*/

int
map_is_night(void)
{
    return map_frame % 0x800 < 0x400;
}



/*
  map animate frame
*/

void
map_animate_frame(void)
{

    /* no animations in buildings */
#ifdef TTT
    if (building_id != -1)
        return;
    if (gui_mode() == DIALOG_BOOK)
        return;
#endif

    /* animate day/night changes */

    if (map_id % 2 == 0 && !gui_mode()) {
        ++map_frame;
        if (map_fade != NIGHT_FADE && map_fade != DAY_FADE) {
            if (map_is_night())
                map_set_fade(map_fade - MAP_FADE_STEP);
            else
                map_set_fade(map_fade + MAP_FADE_STEP);
        }

        if (map_fade == NIGHT_FADE && !map_is_night())
            map_set_fade(map_fade + MAP_FADE_STEP);

        if (map_fade == DAY_FADE && map_is_night())
            map_set_fade(map_fade - MAP_FADE_STEP);
    }

    if ((map_anim_frame++) % 8 != 0)
        return;

    if (map_id % 2 == 0) {
        /* water animation */
        RotatePaletteLeftHidden(0x9, 0xe);
        FadePalette(map_fade, 0x9, 0xe - 0x9 + 1);

        /* fire animation */
        RotatePaletteLeft(0x3c, 0x3f);
    }
    else {
        /* underground water */
        RotatePaletteLeft(0x31, 0x35);

#ifndef TTT
        /* lava in Moria and Dol-Guldur */
        if (map_id == 9 || map_id == 0xd)
            RotatePaletteLeft(0x1c, 0x1e);

        /* fire animation */
        RotatePaletteLeft(0x3c, 0x3f);
#endif
    }

}





/*
  adds character to the map
*/

void
map_add_character(Character * character)
{
    if (map_characters_num + 1 > MAX_CHARACTERS) {
        fprintf(stderr, "lord: too many characters\n");
        exit(1);
    }

    map_characters[map_characters_num][MAP_CHAR_X] = character->x;
    map_characters[map_characters_num][MAP_CHAR_Y] = character->y;
    map_characters[map_characters_num][MAP_CHAR_DIR] = character->direction;
    map_characters[map_characters_num][MAP_CHAR_ID] = character->id;
    character->map_id = map_characters_num++;
}


/*
  adds unique character to the map
*/

void
map_unique_add_character(Character * character)
{
    int i;
    for (i = 0; i < map_characters_num; ++i)
        if (map_characters[i][MAP_CHAR_ID] == character->id) {
            character->map_id = i;
            character->map = map_id;
            map_character_update(character);
            break;
        }

    if (i == map_characters_num)
        map_add_character(character);

}


/*
  remove all characters
*/

void
map_remove_all_characters(void)
{
    map_characters_num = 0;
}



/*
  removes character from the map
*/

void
map_remove_character(int id)
{
    int i;

    for (i = 0; i < map_characters_num; ++i)
        if (map_characters[i][MAP_CHAR_ID] == id)
            map_characters[i][MAP_CHAR_ID] = 0;
}



/*
  updates character position
*/

void
map_character_update(Character * character)
{
    int id = character->map_id;
    map_characters[id][MAP_CHAR_X] = character->x;
    map_characters[id][MAP_CHAR_Y] = character->y;
    map_characters[id][MAP_CHAR_DIR] = character->direction;
}







/*
  get terrain type
*/

int
terrain_get(int x, int y)
{

    int largetile;
    int tile;
    int basictile;

    if (x < 0 || y < 0)
        return 0;

#ifndef TTT
    if (x >= MAP_WIDTH * 16 || y >= MAP_HEIGHT * 16)
        return 0;
    largetile = map_desc[x / 16][y / 16];
    tile = largetiles[largetile][(x % 16) / 4][(y % 16) / 4];
#else
    if (building_id == -1) {
        if (x >= MAP_WIDTH * 16 || y >= MAP_HEIGHT * 16)
            return 0;
        largetile = map_desc[x / 16][y / 16];
        tile = largetiles[largetile][(x % 16) / 4][(y % 16) / 4];
    }
    else {
        if (x >= BUILDING_WIDTH * 4 || y >= BUILDING_HEIGHT * 4)
            return 0;
        tile = building_desc[x / 4][y / 4];
    }
#endif

    basictile = tiles[tile][x % 4][y % 4];

    return tileterrain[basictile];

}





/*
  is terrain passable?
*/

int
terrain_free(int x, int y)
{

    switch (terrain_get(x, y)) {
    case TERRAIN_PATH:
        return 1;
    case TERRAIN_INPASSABLE:
        return 0;
    case TERRAIN_SWAMP:
        return 1;
    case TERRAIN_ROCK:
        return map_climb_mode;
    case TERRAIN_DOWN:
        return 1;
    case TERRAIN_UP:
        return 1;
    case TERRAIN_PIT:
        return 0;
    case TERRAIN_STAIRS_DOWN:
        return 1;
    case TERRAIN_EASY:
        return 1;
    case TERRAIN_HARD:
        return 1;
    case TERRAIN_HARDER:
        return 1;
    case TERRAIN_ICE:
        return 0;
    case TERRAIN_ICEPATH:
        return 1;
    case TERRAIN_WATER:
        return 0;
    case TERRAIN_WATER2:
        return 0;
    }

    return 0;

}


/*
  can character move to (x,y)?
*/

int
map_can_move_to(Character * character, int x, int y)
{
    int i, id;
    int isleader;
    isleader = (character == game_get_leader());

    for (i = 0; i < map_num_disp_char; ++i) {
        if (!combat_get_mode() && isleader &&
            game_in_party(character_get(map_displayed_characters[5 * i + 2])))
            continue;
        id = map_displayed_characters[5 * i + 4];
        if (id == character->map_id)
            continue;

        if (abs(x * 4 - map_characters[id][MAP_CHAR_X]) < 4
            && abs(y * 4 - map_characters[id][MAP_CHAR_Y]) < 4)
            return 0;
    }

    return terrain_free(x, y + 1) &&
        terrain_free(x + 1, y + 1) && terrain_free(x + 2, y + 1);
}



/*
  returns a spot character is standing in
*/

CommandSpot *
map_get_spot(Character * character)
{
    int i;
    int w = 0x1000, h = 0x1000;
    int ww, hh;

    CommandSpot *spot = NULL;

    for (i = 0; i < map_num_spots; ++i)
        if (map_is_in_spot(character, map_spots[i], &ww, &hh)) {
            if (ww < w && hh < h) {
                /* we are choosing the smallest spot */
                w = ww;
                h = hh;
                spot = map_spots[i];
            }
        }
#ifdef EXTENDED
    for (i = 0; i < map_num_pythonspots; ++i)
        if (map_is_in_spot(character, map_pythonspots[i], &ww, &hh)) {
            if (ww < w && hh < h) {
                /* we are choosing the smallest spot */
                w = ww;
                h = hh;
                spot = map_pythonspots[i];
            }
        }
#endif
    return spot;
}


/*
  get a spot by its number
*/

CommandSpot *
map_get_spot_number(int index)
{
    if (index < 0 || index >= map_num_spots) {
        fprintf(stderr, "lord: no such spot (index=%d)\n", index);
        exit(1);
    }

    return map_spots[index];
}





/*
  is character standing in the spot?
*/

int
map_is_in_spot(Character * character, CommandSpot * spot, int *w, int *h)
{

    int sx, sy, sh, sw;
    int i;

    if (spot == NULL)
        return 0;
#ifndef TTT
    if (!spot->flag)
        return 0;
#else /* in TTT spot is valid even after disable */
    if (!spot->flag && spot != game_get_actual_spot())
        return 0;
#endif

    if (spot->map != map_id)
        return 0;

#ifdef TTT
    if (spot->building != building_id)
        return 0;
#endif

    i = 5;
    while (i + 6 < spot->headersize) {
        sx = spot->data[i] + 0x100 * spot->data[i + 1];
        sy = spot->data[i + 2] + 0x100 * spot->data[i + 3];
        sw = spot->data[i + 4];
        sh = spot->data[i + 5];

#ifdef TTT
        if (spot->building != -1)
            sy += spot->floor * 32 * 4 * TILESIZE / 2;
#endif

        if (sh != 0 && sw != 0 && character->x >= sx
            && character->x <= sx + sw && character->y >= sy
            && character->y <= sy + sh) {
            *w = sw;
            *h = sh;
            return 1;
        }
        i += 6;
    }

    return 0;
}




/*
  get npcs who stand near the given character
*/

int
map_npc_stay_near(Character * character, int codes[10], int in_party)
{
    int i;
    int n = 0;

    for (i = 0; i < map_characters_num && n < 10; ++i) {
        if (map_characters[i][MAP_CHAR_ID] &&
            map_characters[i][MAP_CHAR_ID] != character->id &&
            (in_party
             ||
             !game_in_party(character_get(map_characters[i][MAP_CHAR_ID])))) {
            if (!in_party && combat_get_mode()
                && !combat_get_enemy(map_characters[i][MAP_CHAR_ID]))
                continue;

            if (abs(character->x - map_characters[i][MAP_CHAR_X]) <
                MAP_NEAR_DISTANCE
                && abs(character->y - map_characters[i][MAP_CHAR_Y]) <
                MAP_NEAR_DISTANCE) {
                codes[n++] = map_characters[i][MAP_CHAR_ID];
            }
        }
    }

    return n;

}




/*
  get map register
*/

int
map_get_register(int index)
{
    int r;
    if (index < 0 || index > 0x79) {
        fprintf(stderr, "lord: wrong register index %02x\n", index);
        exit(1);
    }

    r = map_registers[index / 8];
    return 1 & (r >> (7 - (index % 8)));

}


/*
  get map obejcts
*/

extern Uint8 *
map_get_objects(void)
{
    return map_objects;
}



/*
  set register
*/

void
map_set_register(int index, int value)
{
    int i, r;
    r = map_get_register(index);
    if (value)
        value = 1;
    if (r == value)
        return;

    i = index / 8;
    r = 1 << (7 - (index % 8));

    if (value)
        map_registers[i] += r;
    else
        map_registers[i] -= r;

}



/*
  teleport character
*/
void
map_character_teleport(Character * character, int rel, int x, int y, int dir,
                       int map)
{

    Character *leader;

    leader = game_get_leader();
    character->actived = 1;

    if (rel) {
        if (x > 0x8000)
            character->x = leader->x + x - 0xffff;
        else
            character->x = leader->x + x;

        if (y > 0x8000)
            character->y = leader->y + y - 0xffff;
        else
            character->y = leader->y + y;

    }
    else {
        if (x != 0xffff)
            character->x = x;
        else
            character->x = leader->x;

        if (y != 0xffff)
            character->y = y;
        else
            character->y = leader->y;
    }

    if (dir != 0xff)
        character->direction = dir;

    if ((map != 0xff && map != map_id) ||
        (map == 0xff && character->map != map_id)) {
        character->map = map;
        map_remove_character(character->id);
    }
    else {
        character->map = map_id;
        map_unique_add_character(character);
    }

    if (character->x < 0)
        character->x = 0;
    if (character->x % 4 != 0) {
        character->x /= 4;
        character->x *= 4;
    }

    if (character->y < 0)
        character->y = 0;
    if (character->y % 4 != 0) {
        character->y /= 4;
        character->y *= 4;
    }

}




/*
  turn character
*/

void
map_character_turn_to(int character_id, int x, int y)
{
    int char_map_id;
    int xx, yy;
    int dir;
    char_map_id = character_get(character_id)->map_id;
    xx = map_characters[char_map_id][MAP_CHAR_X];
    yy = map_characters[char_map_id][MAP_CHAR_Y];

    if (abs(xx - x) > abs(yy - y)) {
        if (x < xx)
            dir = CHARACTER_LEFT;
        else
            dir = CHARACTER_RIGHT;
    }
    else {
        if (y < yy)
            dir = CHARACTER_UP;
        else
            dir = CHARACTER_DOWN;
    }

    map_characters[char_map_id][MAP_CHAR_DIR] = dir;

}



/*
  move character
*/

int
map_character_move(int character_id, int x, int y, int dir)
{
    Character *character;
    int char_map_id;

    if (character_id < 0xf0)
        character = character_get(character_id);
    else
        character = game_get_leader();

    character_frame(character);

    if (character->action == CHARACTER_STAY) {
        character->actived = 1;
        char_map_id = character->map_id;

        character->x = map_characters[char_map_id][MAP_CHAR_X];
        character->y = map_characters[char_map_id][MAP_CHAR_Y];
        character->direction = map_characters[char_map_id][MAP_CHAR_DIR];

        if (x > 0x8000)
            x = x - 0xffff + character->x;

        if (y > 0x8000)
            y = y - 0xffff + character->y;



        if (abs(x - character->x) < 4 && abs(y - character->y) < 4) {
            if (dir >= 0 && dir < 4)
                character->direction = dir;
            map_character_update(character);
            return 0;
        }

        if (character->y != y) {
            if (y < character->y)
                character_move_up(character);
            else
                character_move_down(character);
        }
        else {
            if (x < character->x)
                character_move_left(character);
            else
                character_move_right(character);
        }

    }

    if (character == game_get_leader())
        game_follow_leader();

    return 1;

}


/*
  set map mode
*/

void
map_set_mode(int mode, int param)
{
    map_mode = mode;
    map_mode_param = param;
    map_display(game_get_leader()->x * 2, game_get_leader()->y * 2);
}



/*
  get map id
*/

int
map_get_id(void)
{
    return map_id;
}




/*
  set attacking character
*/

void
map_attacking_character(int id)
{
    int i, new_id;
    int x, y, dir;
    Character *character;
    character = character_get(id);

    for (i = 0; i < map_characters_num; ++i)
        if (map_characters[i][MAP_CHAR_ID] == id) {
            x = map_characters[i][MAP_CHAR_X];
            y = map_characters[i][MAP_CHAR_Y];
            dir = map_characters[i][MAP_CHAR_DIR];

            if (2 * x - map_last_x > -3 * (map_width + 1) / 4 &&
                2 * x - map_last_x < 3 * (map_width + 1) / 4 &&
                2 * y - map_last_y > -3 * (map_height + 1) / 4 &&
                2 * y - map_last_y < 3 * (map_height + 1) / 4) {
                new_id = combat_enemy(character, x, y, dir, i);
                map_characters[i][MAP_CHAR_ID] = new_id;
            }
        }

}




/*
  set climb mode
*/

void
map_set_climb_mode(int mode)
{
    map_climb_mode = mode;
}



/*
  set map light mode
*/

void
map_set_light(int mode)
{

    if (map_light_mode == 0 || map_light_mode == 1 || mode == 2)
        map_light_mode = mode;

    if (map_light_mode && map_mode == MAP_DARK)
        map_mode = MAP_LIGHTED;

    if (!map_light_mode && map_mode == MAP_LIGHTED)
        map_mode = MAP_DARK;

}

/*
  set dark map
 */

void
map_set_dark(void)
{
    map_mode = MAP_DARK;
    map_set_light(map_light_mode);
}


/*
  is map dark?
*/

int
map_is_dark(void)
{
    return map_mode == MAP_DARK;
}


/*
  get map center
*/

void
map_get_center(int *x, int *y)
{
    *x = map_last_x;
    *y = map_last_y;
}



/*
  get map building_id
*/

int
map_get_building_id(void)
{
    return building_id;
}



#ifdef EXTENDED
/*
  add new python spot
*/
void
map_add_pythonspot(int map, int x, int y, int w, int h)
{
    CommandSpot *spot;

    if (map_num_pythonspots == MAX_PYTHON_SPOTS) {
        fprintf(stderr, "lord: too many puthon spots.");
        exit(1);
    }

    spot = lordmalloc(sizeof(CommandSpot));

    spot->id = map_num_pythonspots;
    spot->pythonspot = 1;
    spot->flag = 1;
    spot->map = map;
    spot->building = -1;
    spot->floor = 0;
    spot->not_parsed = 0;
    spot->pos = 0;
    spot->commands_num = 0;

    spot->data_size = 12;
    spot->headersize = 12;
    spot->data = lordmalloc(12);
    spot->data[0] = 0xc;
    spot->data[1] = 0;
    spot->data[2] = 0xc;
    spot->data[3] = 0;
    spot->data[4] = map;
    spot->data[5] = x % 0x100;
    spot->data[6] = x / 0x100;
    spot->data[7] = y % 0x100;
    spot->data[8] = y / 0x100;
    spot->data[9] = w;
    spot->data[10] = h;
    spot->data[11] = 0xff;

    map_pythonspots[map_num_pythonspots] = spot;
    map_num_pythonspots++;

}


/*
   get python spot
*/
CommandSpot *
map_get_pythonspot(int id)
{
    return map_pythonspots[id];
}

#endif
