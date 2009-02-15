/****************************************************************************

    shape.c
    shapes manipulation


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
#include "graphics.h"
#include "shape.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define SHAPES_NUM 700

Shape *shapes_cache[SHAPES_NUM];

/* palette for shapes */
Palette *shapes_palette;


/* portraits */
#ifndef TTT
#define PORTRAITS_NUM 46
#else
#define PORTRAITS_NUM 67
#endif
Portrait *portraits_cache[PORTRAITS_NUM];


#ifndef TTT
#include "shapes_param.h"
#else
#include "shapes_param_ttt.h"
#endif


/*
  init shapes
*/

void
shapes_init(void)
{
    Archive *archive;
    FILE *pal;
    Uint8 *data;
    int size;
    Pixmap *pixmap;
    int i, j;
    int w, h;


    pal = lord_fopen("shapes.pal", "rb");
    if (filelen(pal) != sizeof(Palette)) {
        fprintf(stderr, "lord: arts.pal is not a valid palette file\n");
        exit(1);
    }

    shapes_palette = lord_malloc(sizeof(Palette));

    if (fread(shapes_palette, sizeof(Palette), 1, pal) != 1) {
        fprintf(stderr, "lord: can not read arts.pal\n");
        exit(1);
    }

    fclose(pal);

    /* shapes palette is stored in 0..0x19 and is mapped to 0x40..0x59 */

    shapes_set_palette();

    archive = ndxarchiveopen("shapes");

    if (archive->size != SHAPES_NUM) {
        fprintf(stderr, "lord: expecting %d shapes (%d found)\n", SHAPES_NUM,
                archive->size);
        exit(1);
    }

    w = 16;

    for (i = 0; i < archive->size; ++i)
        if (archivedatasize(archive, i) == 0 || shapes_param[i][0] == 0) {
            shapes_cache[i] = NULL;
        }
        else {
            data = ndxdecompressarchive(archive, i, &size);
            shapes_cache[i] = lord_malloc(sizeof(Shape));

            w = shapes_param[i][0];
            h = size / shapes_param[i][0] / shapes_param[i][1];

            for (j = 0; j < 4; ++j)
                shapes_cache[i]->pixmaps[j] = NULL;

            shapes_cache[i]->pixmaps_num = shapes_param[i][1];

            for (j = 0; j < shapes_cache[i]->pixmaps_num; ++j) {
                pixmap = pixmap_new(w, h);
                memcpy(pixmap->data, data + j * w * h, w * h);
                pixmap_setalpha(pixmap, 0x9e);
                shapes_cache[i]->pixmaps[j] = pixmap;
            }

            free(data);

        }

    archiveclose(archive);





    /* init portraits cache */

    archive = ndxarchiveopen("portrait");

    if (archive->size != PORTRAITS_NUM) {
        fprintf(stderr, "lord: expecting %d portraits (%d found)\n",
                PORTRAITS_NUM, archive->size);
        exit(1);
    }

    for (i = 0; i < PORTRAITS_NUM; ++i) {
        portraits_cache[i] = NULL;
        if (archivedatasize(archive, i) == 0)
            continue;
        data = ndxdecompressarchive(archive, i, &size);

        if (size == 4970 || size == 3768) {
            pixmap = lord_malloc(sizeof(Pixmap));
            pixmap->width = 68;
            pixmap->height = 54;
            pixmap->datasize = pixmap->width * pixmap->height;
            pixmap->hasalpha = 0;
            pixmap->data = data;


            portraits_cache[i] = lord_malloc(sizeof(Portrait));

            portraits_cache[i]->pixmap = pixmap;


            if (size == 4970) {
                portraits_cache[i]->freepalette = 0;
                portraits_cache[i]->palette =
                    (Palette *)(data + pixmap->datasize);

                /* portraits colors are 0x60--0x80 */
                palette_copy_colors(portraits_cache[i]->palette, 0, 0x20,
                                    0x60);

            }
            else {
                portraits_cache[i]->freepalette = 0;
                portraits_cache[i]->palette = lord_malloc(sizeof(Palette));

                /* portraits palette is stored in 0x60--0x80 */
                memcpy((void *)portraits_cache[i]->palette + 3 * 0x60,
                       data + pixmap->datasize, 0x60);
            }

        }
    }

    archiveclose(archive);







}






/*
  close shapes
*/

void
shapes_close(void)
{
    int i, j;

    for (i = 0; i < SHAPES_NUM; ++i)
        if (shapes_cache[i] != NULL) {
            for (j = 0; j < 4; ++j)
                if (shapes_cache[i]->pixmaps[j] != NULL)
                    pixmap_free(shapes_cache[i]->pixmaps[j]);

            free(shapes_cache[i]);
        }




    /* free portraits cache */

    for (i = 0; i < PORTRAITS_NUM; ++i) {
        if (portraits_cache[i] == NULL)
            continue;
        if (portraits_cache[i]->freepalette)
            free(portraits_cache[i]->palette);
        free(portraits_cache[i]->pixmap);
        free(portraits_cache[i]);
        portraits_cache[i] = NULL;
    }



}




/*
  set the shapes palette
*/

void
shapes_set_palette(void)
{
    SetPalette(shapes_palette, 0x80, 0x40);
}




/*
  get shape
*/

Shape *
shape_get(int i)
{
    if (i < 0 || i >= SHAPES_NUM)
        return NULL;

    return shapes_cache[i];
}





/*
  draw shape
*/

void
shape_draw(Shape *shape, int state, int x, int y)
{

    Pixmap *pixmap;

    if (shape == NULL)
        return;

    if (shape->pixmaps[0] == NULL)
        return;

    if (shape->pixmaps[state] == NULL)
        pixmap = shape->pixmaps[0];
    else
        pixmap = shape->pixmaps[state];


    pixmap_draw(pixmap, x - pixmap->width, y - pixmap->height);

}







/*
  draw portrait
*/

void
portrait_draw(int index, int x, int y)
{

    if (index < 0 || index >= PORTRAITS_NUM || portraits_cache[index] == NULL) {
        fprintf(stderr, "lord: invalid portrait num %d\n", index);
        // FIXME: exit(1);
        return;
    }

    /* portraits colors are 0x60--0x80 */

    SetPalette(portraits_cache[index]->palette, 0x60, 0x20);
    pixmap_draw(portraits_cache[index]->pixmap, x, y);

}



/*
  get weapon shape corresponding to character shape
*/

int
shape_get_weapon_shape_id(int character_shape_id, int weapon_class)
{
    int weapon_bank = -1;

    if (shapes_cache[6 * character_shape_id + 4] == NULL)
        return -1;

    if (character_shape_id == 0 ||      /* hobbit */
        character_shape_id == 3 ||      /* elf */
        character_shape_id == 4 ||      /* man in green */
        character_shape_id == 5 ||      /* dwarf */
        character_shape_id == 12 ||     /* Bombadil */
        character_shape_id == 13 ||     /* Gandalf */
        character_shape_id == 16 ||     /* Freddi */
        character_shape_id == 17 ||     /* Gaffer or green hobbit in TTT */
        character_shape_id == 22 ||     /* Elf woman */
        character_shape_id == 25 ||     /* Man in blue */
        character_shape_id == 32 ||     /* Woman */
        character_shape_id == 35 ||     /* Nazgul */
        character_shape_id == 98 ||     /* white hobbit */
        character_shape_id == 101 ||    /* Legolas */
        character_shape_id == 110 ||    /* red man */
        character_shape_id == 115 ||    /* red hobbit */
        0)
        weapon_bank = 222;

    if (character_shape_id == 19)
        weapon_bank = 336;      /* orc */
    if (character_shape_id == 21)
        weapon_bank = 456;      /* troll */
    if (character_shape_id == 23)
        weapon_bank = 396;      /* Gollum */
#ifdef TTT                      /* there is one shape missing in vol 1 */
    if (character_shape_id == 31)
        weapon_bank = 576;      /* ent */
#endif
    if (character_shape_id == 34)
        weapon_bank = 516;      /* ghost */



    if (weapon_bank != -1
        && shapes_cache[weapon_bank + 6 * weapon_class] != NULL)
        return weapon_bank + 6 * weapon_class;

    return -1;
}
