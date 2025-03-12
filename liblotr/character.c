/****************************************************************************

    character.c
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


#include "lotr.h"
#include "archive.h"
#include "character.h"
#include "combat.h"
#include "game.h"
#include "map.h"
#include "object.h"
#include "shape.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>


#ifndef TTT
#include "characters_pos.h"
#else
#include "characters_pos_ttt.h"
#endif

Character *lotr_characters[CHARACTERS_NUM];



typedef struct
{
    Uint8 unknown1[6];
    Uint8 shape;                /* 6 */
    Uint8 unknown6[3];          /* 7 */
    Uint8 direction;            /* 10 */
    Uint8 unknown7[2];          /* 11 */
    Uint8 silver;               /* 13 */
    Uint8 horse_shape;          /* 14 */
    Uint8 unknown8;             /* 15 */
    Uint8 dex;                  /* 16 */
    Uint8 life;                 /* 17 */
    Uint8 str;                  /* 18 */
    Uint8 will;                 /* 19 */
    Uint8 unknown4;             /* 20 */
    Uint8 luck;                 /* 21 */
    Uint8 end;                  /* 22 */
    Uint8 unknown3;             /* 23 */
    Uint8 items[10];            /* 24 */
    Uint8 skills[10];           /* 34 */
    Uint8 spells[10];           /* 44 */
    Uint8 name[16];             /* 54 */
    Uint8 id;                   /* 70 */
    Uint8 unknown2[9];          /* 71 */
    Uint8 portrait;             /* 80 */
    Uint8 unknown5[6];          /* 81 */
    Uint8 talks[CHARACTER_TALK_LEN];    /* 87 */
} NpcsDatEntry;



int weapon_offsets[36][4][2] = { {{7, -3}, {10, 1}, {3, 5}, {0, 4},},   /*  0 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /*  1 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /*  2 */
{{1, -6}, {6, 2}, {-1, 4}, {-3, 3},},   /*  3 */
{{6, -5}, {10, -3}, {3, 3}, {1, -4},},  /*  4 */
{{6, -4}, {9, -2}, {2, 3}, {-1, 0},},   /*  5 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /*  6 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /*  7 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /*  8 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /*  9 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 10 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 11 */
{{7, -4}, {8, 2}, {2, 3}, {-2, 4},},    /* 12 */
{{6, -2}, {7, -1}, {0, 1}, {0, 2},},    /* 13 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 14 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 15 */
{{8, 2}, {7, 3}, {3, 8}, {1, 4},},      /* 16 */
{{8, 0}, {9, 2}, {3, 5}, {0, 4},},      /* 17 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 18 */
{{8, -2}, {14, 1}, {2, 6}, {1, 4},},    /* 19 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 20 */
{{4, -8}, {4, 0}, {-2, 1}, {-9, -5},},  /* 21 */
{{9, -1}, {8, 3}, {2, 6}, {1, 3},},     /* 22 */
{{9, 1}, {11, 1}, {2, 6}, {4, 3},},     /* 23 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 24 */
{{7, -4}, {9, 3}, {4, 5}, {0, 3},},     /* 25 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 26 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 27 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 28 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 29 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 30 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 31 */
{{9, -2}, {7, 2}, {3, 4}, {0, 4},},     /* 32 */
{{0, 0}, {0, 0}, {0, 0}, {0, 0},},      /* 33 */
{{9, -3}, {8, 0}, {0, 2}, {0, 2},},     /* 34 */
{{7, -3}, {6, 4}, {2, 4}, {-3, 1},}
};                              /* 35 */




void
character_set_shape(Character *character, int character_id, int shape_id)
{
    character->shape_id = shape_id;
    character->shapes[0] = shape_get(6 * shape_id + 0);
    character->shapes[1] = shape_get(6 * shape_id + 1);
    character->shapes[2] = shape_get(6 * shape_id + 2);
    character->shapes[3] = shape_get(6 * shape_id + 3);
    character->shapes[4] = shape_get(6 * shape_id + 4);
    character->shapes[5] = shape_get(6 * shape_id + 5);

    int is_composite = FALSE;
#ifndef TTT
    is_composite = (character_id == 0xc3); /* Balrog */
#else
    is_composite = (character_id == 0x23)  /* Mumak */
                || (character_id == 0xb6); /* Dragon */
#endif
    character->composite_shape = is_composite;
    if (is_composite) {
        character->shapes[6] =  shape_get(6 * shape_id + 6);
        character->shapes[7] =  shape_get(6 * shape_id + 7);
        character->shapes[8] =  shape_get(6 * shape_id + 8);
        character->shapes[9] =  shape_get(6 * shape_id + 9);
        character->shapes[10] = shape_get(6 * shape_id + 10);
        character->shapes[11] = shape_get(6 * shape_id + 11);
    } else {
        character->shapes[6] =  NULL;
        character->shapes[7] =  NULL;
        character->shapes[8] =  NULL;
        character->shapes[9] =  NULL;
        character->shapes[10] = NULL;
        character->shapes[11] = NULL;
    }
}


/*
  init characters & portraits
*/

void
characters_init()
{

    int i, j;
    NpcsDatEntry *chardata;

    Archive *archive;

#ifndef DEMO
    archive = archive_ndx_open("nnpcs");
#else
    archive = archive_ndx_open("npcs");
#endif

    if (archive->size != CHARACTERS_NUM) {
        fprintf(stderr, "lotr: expecting %d game characters (%d found)\n",
                CHARACTERS_NUM, archive->size);
        exit(1);
    }


    if (sizeof(NpcsDatEntry) != 256) {
        fprintf(stderr, "lotr: internal error\n");
        exit(1);
    }

    for (i = 0; i < CHARACTERS_NUM; ++i) {

        if (archive_data_size(archive, i) != 256) {
            lotr_characters[i] = NULL;
            continue;
        }

        chardata = (NpcsDatEntry *) archive_read(archive, i);

        lotr_characters[i] = lotr_malloc(sizeof(Character));
        lotr_characters[i]->x = -1;
        lotr_characters[i]->y = -1;
        lotr_characters[i]->direction = CHARACTER_DOWN;
        lotr_characters[i]->action = CHARACTER_STAY;
        lotr_characters[i]->state = 0;
        lotr_characters[i]->x_offset = 0;
        lotr_characters[i]->y_offset = 0;

        character_set_shape(lotr_characters[i], i, chardata->shape);
        lotr_characters[i]->horse_shape_id = chardata->horse_shape;
        lotr_characters[i]->portrait = chardata->portrait;
        lotr_characters[i]->direction = chardata->direction;
#ifdef CD_VERSION
        /* directions in npc data seem to be in range 0--4, strange... */
        if (lotr_characters[i]->direction == 4)
            lotr_characters[i]->direction = CHARACTER_DOWN;
#endif

        lotr_characters[i]->silver = chardata->silver;
        lotr_characters[i]->dex = chardata->dex;
        lotr_characters[i]->end = chardata->end;
        lotr_characters[i]->life = chardata->life;
        lotr_characters[i]->str = chardata->str;
        lotr_characters[i]->luck = chardata->luck;
        lotr_characters[i]->will = chardata->will;

        lotr_characters[i]->id = i;     /* chardata->id; */
        lotr_characters[i]->party_id = 0xff;
        lotr_characters[i]->actived = 0;
        lotr_characters[i]->converted = 0;
        lotr_characters[i]->last_eat = -0x1000;

        lotr_characters[i]->texts[0] = 0x0;
        lotr_characters[i]->texts[1] = 0xff;

        lotr_characters[i]->willing_join = (i >= 0xa0 && i <= 0xa3) ||  /* hobbits */
            i == 0xa5 ||        /* Gandalf */
            i == 0xa6 ||        /* Gimli */
            i == 0xa7 ||        /* Legolas */
#ifndef TTT
            i == 0xa8 ||        /* Boromir */
            i == 0x01 ||        /* Kiri */
            i == 0xbe ||        /* Bilbo */
            i == 0xc5 ||        /* Celeborn */
            i == 0xca ||        /* Athelwyn */
            i == 0xef ||        /* false Radagast */
#else
#endif
            i == 0xff;



        strncpy(lotr_characters[i]->name, (char *)chardata->name, 16);
        lotr_characters[i]->name[16] = 0;
        strncpy(lotr_characters[i]->original_name, (char *)chardata->name, 16);
        lotr_characters[i]->original_name[16] = 0;
#ifdef TTT
        if (strncmp(lotr_characters[i]->name, "door", 19) == 0
            || strncmp(lotr_characters[i]->name, "Door", 19) == 0)
        {
            lotr_characters[i]->x_offset = -15;
            lotr_characters[i]->y_offset = -5;
        }
#endif

        lotr_characters[i]->ring_mode = 0;

        for (j = 0; chardata->items[j] != 0xff && j < 10; ++j) {
            lotr_characters[i]->items[j] = chardata->items[j];
            lotr_characters[i]->item_used[j] = 0;
            if (object_is_ring(lotr_characters[i]->items[j]))
                lotr_characters[i]->ring_mode = 1;
        }

        lotr_characters[i]->items_num = j;


        for (j = 0; chardata->spells[j] != 0xff && j < 10; ++j)
            lotr_characters[i]->spells[j] = chardata->spells[j];

        lotr_characters[i]->spells_num = j;


        for (j = 0; chardata->skills[j] != 0xff && j < 10; ++j)
            lotr_characters[i]->skills[j] = chardata->skills[j];

        lotr_characters[i]->skills_num = j;

#ifdef DEBUG
        printf("%02x -- %-20s", i, lotr_characters[i]->name);
        if (lotr_characters[i]->items_num) {
            printf(" (");
            for (j = 0; j < lotr_characters[i]->items_num; ++j) {
                printf("%s", object_name(lotr_characters[i]->items[j]));
                if (j + 1 < lotr_characters[i]->items_num)
                    printf(", ");
                else
                    printf(")");
            }
        }
        printf("\n");
#endif



    }

    archive_close(archive);

#ifdef TTT
    for (i = 0; characters_pos[i][0] != 0xffff; ++i) {
        lotr_characters[characters_pos[i][3]]->party_id =
            characters_pos[i][8];
        lotr_characters[characters_pos[i][3]]->x = characters_pos[i][0];
        lotr_characters[characters_pos[i][3]]->y = characters_pos[i][1];
        lotr_characters[characters_pos[i][3]]->map = characters_pos[i][2];
    }
#endif



}




/*
  save characters
*/

void
characters_save(xmlNodePtr node)
{
    int i;
    int k;
    Uint8 buf[10];

    xmlNodePtr cur;

    for (i = 0; i < CHARACTERS_NUM; ++i)
        if (lotr_characters[i]) {
            cur = xmlNewNode(NULL, (const xmlChar *)"character");

            lotr_save_prop_int(cur, "id", i);

            xmlNewTextChild(cur, NULL, (const xmlChar *)"name",
                            (const xmlChar *)lotr_characters[i]->name);

            lotr_save_prop_int(cur, "x", lotr_characters[i]->x);
            lotr_save_prop_int(cur, "y", lotr_characters[i]->y);
            lotr_save_prop_int(cur, "map", lotr_characters[i]->map);
            lotr_save_prop_int(cur, "dir", lotr_characters[i]->direction);
            lotr_save_prop_int(cur, "action", lotr_characters[i]->action);
            lotr_save_prop_int(cur, "state", lotr_characters[i]->state);
            lotr_save_prop_int(cur, "shape", lotr_characters[i]->shape_id);
            lotr_save_prop_int(cur, "horse_shape",
                               lotr_characters[i]->horse_shape_id);
            lotr_save_prop_int(cur, "portrait", lotr_characters[i]->portrait);

            lotr_save_prop_int(cur, "silver", lotr_characters[i]->silver);
            lotr_save_prop_int(cur, "dex", lotr_characters[i]->dex);
            lotr_save_prop_int(cur, "end", lotr_characters[i]->end);
            lotr_save_prop_int(cur, "life", lotr_characters[i]->life);
            lotr_save_prop_int(cur, "str", lotr_characters[i]->str);
            lotr_save_prop_int(cur, "luck", lotr_characters[i]->luck);
            lotr_save_prop_int(cur, "will", lotr_characters[i]->will);

#ifdef TTT
            lotr_save_prop_int(cur, "party", lotr_characters[i]->party_id);
#endif
            lotr_save_prop_int(cur, "actived", lotr_characters[i]->actived);
            lotr_save_prop_int(cur, "last_eat", lotr_characters[i]->last_eat);
            lotr_save_prop_int(cur, "willing_join",
                               lotr_characters[i]->willing_join);
            lotr_save_prop_int(cur, "ring_mode",
                               lotr_characters[i]->ring_mode);

            lotr_save_prop_field(cur, "texts",
                                 (Uint8 *)lotr_characters[i]->texts,
                                 CHARACTER_TALK_LEN);

            for (k = 0; k < 10; ++k)
                buf[k] = lotr_characters[i]->items[k];
            lotr_save_prop_field(cur, "items", buf,
                                 lotr_characters[i]->items_num);
            for (k = 0; k < 10; ++k)
                buf[k] = lotr_characters[i]->item_used[k];
            lotr_save_prop_field(cur, "item_used", buf,
                                 lotr_characters[i]->items_num);

            for (k = 0; k < 10; ++k)
                buf[k] = lotr_characters[i]->skills[k];
            lotr_save_prop_field(cur, "skills", buf,
                                 lotr_characters[i]->skills_num);

            for (k = 0; k < 10; ++k)
                buf[k] = lotr_characters[i]->spells[k];
            lotr_save_prop_field(cur, "spells", buf,
                                 lotr_characters[i]->spells_num);

            xmlAddChild(node, cur);
        }
}


/*
  load characters
 */

void
characters_load(xmlNodePtr node)
{
    xmlNodePtr cur;
    int i, k;
    Uint8 buf[10];
    char *name;

    cur = node->xmlChildrenNode;

    while (cur != NULL) {
        if (xmlNodeIsText(cur)) {
            cur = cur->next;
            continue;
        }
        i = lotr_load_prop_int(cur, "id");
        if (i >= 0 && i < CHARACTERS_NUM && lotr_characters[i]) {

            name = (char *)
                xmlNodeGetContent(lotr_get_subnode
                                  (cur, (const xmlChar *)"name", 1));
            if (strncmp(name, "Aragorn", 19) && strncmp(name, "Radagast", 19)
                && strncmp(name, "Werewolf", 19))
                strncpy(lotr_characters[i]->name,
                        lotr_characters[i]->original_name, 20);
            else
                strncpy(lotr_characters[i]->name, name, 19);

            lotr_characters[i]->x = lotr_load_prop_int(cur, "x");
            lotr_characters[i]->y = lotr_load_prop_int(cur, "y");
            lotr_characters[i]->map = lotr_load_prop_int(cur, "map");
            lotr_characters[i]->direction = lotr_load_prop_int(cur, "dir");
            lotr_characters[i]->action = lotr_load_prop_int(cur, "action");
            lotr_characters[i]->state = lotr_load_prop_int(cur, "state");

            character_set_shape(lotr_characters[i], i, lotr_load_prop_int(cur, "shape"));

            lotr_characters[i]->horse_shape_id =
                lotr_load_prop_int(cur, "horse_shape");
            lotr_characters[i]->portrait =
                lotr_load_prop_int(cur, "portrait");

            lotr_characters[i]->silver = lotr_load_prop_int(cur, "silver");
            lotr_characters[i]->dex = lotr_load_prop_int(cur, "dex");
            lotr_characters[i]->end = lotr_load_prop_int(cur, "end");
            lotr_characters[i]->life = lotr_load_prop_int(cur, "life");
            lotr_characters[i]->str = lotr_load_prop_int(cur, "str");
            lotr_characters[i]->luck = lotr_load_prop_int(cur, "luck");
            lotr_characters[i]->will = lotr_load_prop_int(cur, "will");

#ifdef TTT
            lotr_characters[i]->party_id = lotr_load_prop_int(cur, "party");
#endif
            lotr_characters[i]->actived = lotr_load_prop_int(cur, "actived");
            lotr_characters[i]->last_eat =
                lotr_load_prop_int(cur, "last_eat");
            lotr_characters[i]->willing_join =
                lotr_load_prop_int(cur, "willing_join");
            lotr_characters[i]->ring_mode =
                lotr_load_prop_int(cur, "ring_mode");

            lotr_load_prop_field(cur, "texts",
                                 (Uint8 *)lotr_characters[i]->texts,
                                 CHARACTER_TALK_LEN);


            lotr_characters[i]->items_num =
                lotr_load_prop_field(cur, "items", buf, 10);
            for (k = 0; k < lotr_characters[i]->items_num; ++k)
                lotr_characters[i]->items[k] = buf[k];
            lotr_characters[i]->items_num =
                lotr_load_prop_field(cur, "item_used", buf, 10);
            for (k = 0; k < lotr_characters[i]->items_num; ++k)
                lotr_characters[i]->item_used[k] = buf[k];

            lotr_characters[i]->skills_num =
                lotr_load_prop_field(cur, "skills", buf, 10);
            for (k = 0; k < lotr_characters[i]->skills_num; ++k)
                lotr_characters[i]->skills[k] = buf[k];

            lotr_characters[i]->spells_num =
                lotr_load_prop_field(cur, "spells", buf, 10);
            for (k = 0; k < lotr_characters[i]->spells_num; ++k)
                lotr_characters[i]->spells[k] = buf[k];

        }

        cur = cur->next;

    }
}



#ifdef TTT
/*
  convert character
*/

void
character_convert(xmlNodePtr node, int index)
{
    xmlNodePtr cur;
    Uint8 item_buf[10];
    Uint8 item_used_buf[10];
    int i, k, l;

    cur = node->xmlChildrenNode;

    while (cur != NULL) {
        if (xmlNodeIsText(cur)) {
            cur = cur->next;
            continue;
        }
        i = lotr_load_prop_int(cur, "id");
        if (i == index) {
            if (i >= 0 && i < CHARACTERS_NUM && lotr_characters[i]) {
                lotr_characters[i]->dex = lotr_load_prop_int(cur, "dex");
                lotr_characters[i]->end = lotr_load_prop_int(cur, "end");
                lotr_characters[i]->life = lotr_characters[i]->end;
                lotr_characters[i]->str = lotr_load_prop_int(cur, "str");
                lotr_characters[i]->luck = lotr_load_prop_int(cur, "luck");
                lotr_characters[i]->will = lotr_load_prop_int(cur, "will");

                lotr_characters[i]->converted = 1;

                lotr_characters[i]->items_num =
                    lotr_load_prop_field(cur, "items", item_buf, 10);
                lotr_load_prop_field(cur, "item_used", item_used_buf, 10);

                for (k = 0, l = 0; k < lotr_characters[i]->items_num; ++k) {
                    if (object_transferable(item_buf[k])) {
                        lotr_characters[i]->items[l] = item_buf[k];
                        lotr_characters[i]->item_used[l] = item_used_buf[k];
                        ++l;
                    }
                }

                lotr_characters[i]->items_num = l;

            }
            return;
        }

        cur = cur->next;

    }

}
#endif



/*
  close characters
 */

void
characters_close()
{

    int i;

    for (i = 0; i < CHARACTERS_NUM; ++i)
        if (lotr_characters[i])
            free(lotr_characters[i]);

}




/*
  character type exists
*/

int
character_exists(int i)
{

    if (i >= 0x100)
        return combat_get_enemy(i) != NULL;

    if (i < 0 || i >= CHARACTERS_NUM || lotr_characters[i] == NULL)
        return 0;
    return 1;
}



/*
  get character type
*/

Character *
character_get(int i)
{

    if (!character_exists(i)) {
        fprintf(stderr, "lotr: no such character (index=%02x)\n", i);
        exit(1);
    }

    if (i >= 0x100)
        return combat_get_enemy(i);

    return lotr_characters[i];
}



/*
  put characters to the map
*/

void
character_fill_map(int map_num, int building)
{
    int i;
    Character *character;

    map_remove_all_characters();

    for (i = 0; characters_pos[i][0] != 0xffff; ++i) {
        character = lotr_characters[characters_pos[i][3]];
        if (!character->actived && characters_pos[i][2] == map_num)
#ifdef TTT
            if (characters_pos[i][6] == building)
#endif
            {
                character->x = characters_pos[i][0];
                character->y = characters_pos[i][1];
#ifdef TTT
                if (building != 0xff)
                    character->y += characters_pos[i][7] * 4 * 4 * 32;
#endif
                character->map = characters_pos[i][2];
                map_add_character(character);
            }
        if (character->actived && character->map == map_num) {
            map_unique_add_character(character);
        }
    }

}



/*
  draw character
*/


void
character_draw(int id, int x, int y, int dir)
{
    int frame;
    Character *character;
    Shape *horse1;
    Shape *horse2;
    int horse, xoffset, yoffset;
    int char_shape;

    character = character_get(id);

    if (dir < 0 || dir > 3) {
        fprintf(stderr, "lotr: wrong direction in character_draw.\n");
        exit(1);
    }


    if (character->horse_shape_id != 0xff) {
        horse = character->horse_shape_id * 6;
        if (character->action == CHARACTER_STAY) {
            horse = horse + CHARACTER_STAY;
            frame = dir;
        } else if (character->action == CHARACTER_ATTACK) {
            horse = horse + dir;
            frame = dir;
        } else {
            horse = horse + dir;
            frame = character->state;
        }

        horse1 = shape_get(horse);
        horse2 = shape_get(horse + 6);

        xoffset = yoffset = 0;

        switch (dir) {
            case CHARACTER_UP:
            case CHARACTER_DOWN:
                xoffset = 2;
                yoffset = -4;
                break;

            case CHARACTER_RIGHT:
            case CHARACTER_LEFT:
                xoffset = -10;
                yoffset = 4;
                break;
        }

        shape_draw(horse1, frame, x + xoffset, y + yoffset);
        if (dir == CHARACTER_UP)
            shape_draw(horse2, frame, x + xoffset,
                       y + yoffset + horse2->pixmaps[frame]->height);
        if (dir == CHARACTER_DOWN)
            shape_draw(horse2, frame, x + xoffset,
                       y + yoffset + horse2->pixmaps[frame]->height);
        if (dir == CHARACTER_LEFT)
            shape_draw(horse2, frame,
                       x + xoffset + horse2->pixmaps[frame]->width,
                       y + yoffset);
        if (dir == CHARACTER_RIGHT)
            shape_draw(horse2, frame,
                       x + xoffset + horse2->pixmaps[frame]->width,
                       y + yoffset);


    }


    if (character->ring_mode == 2 || character->shape_id == 0xff)
        return;

    if (character->action == CHARACTER_ATTACK) {
        if (character->weapon_shape_id < 0) {
            shape_draw(character->shapes[dir], 2, x, y);
        } else {
            if (dir == CHARACTER_DOWN || dir == CHARACTER_RIGHT)
                shape_draw(character->shapes[CHARACTER_ATTACK], dir, x, y);

            char_shape = character->shape_id;

#ifdef TTT
            if (char_shape == 98 || char_shape == 17 || char_shape == 115)
                char_shape = 0;
            if (char_shape == 110)
                char_shape = 4;
            if (char_shape == 101)
                char_shape = 3;
#endif

            if (char_shape > 35)
                char_shape = 0;

            shape_draw(shape_get(character->weapon_shape_id + dir),
                       character->state / 2,
                       x + weapon_offsets[char_shape][dir][0],
                       y + weapon_offsets[char_shape][dir][1]);

            if (dir == CHARACTER_UP || dir == CHARACTER_LEFT)
                shape_draw(character->shapes[CHARACTER_ATTACK], dir, x, y);
        }
        return;
    }

    int draw_action = character->action;
    if (character->shapes[draw_action] == NULL)
        draw_action = CHARACTER_STAY;

    if (draw_action == CHARACTER_STAY)
        frame = dir;
    else
        frame = character->state;

    int xx = x + character->x_offset;
    int yy = y + character->y_offset;
    shape_draw(character->shapes[draw_action], frame, xx, yy);
    if (character->composite_shape) {
#ifndef TTT
        if (dir == CHARACTER_UP || dir == CHARACTER_DOWN) {
            int off = character->shapes[draw_action]->pixmaps[frame]->width;
            shape_draw(character->shapes[draw_action + 6], frame, xx + off, yy);
        } else {
            int off = character->shapes[draw_action]->pixmaps[frame]->width;
            shape_draw(character->shapes[draw_action + 6], frame, xx, yy + off);
        }
#else
        if (dir == CHARACTER_UP || dir == CHARACTER_DOWN) {
            int off = character->shapes[draw_action]->pixmaps[frame]->height;
            shape_draw(character->shapes[draw_action + 6], frame, xx, yy - off);
        } else {
            int off = character->shapes[draw_action]->pixmaps[frame]->width;
            shape_draw(character->shapes[draw_action + 6], frame, xx + off, yy);
        }
#endif
    }
}





/*
  set character attack
*/

void
character_attack(Character *character, int direction)
{
    int i, weapon;
    if (character->action == CHARACTER_STAY) {
        character->direction = direction;
        character->action = CHARACTER_ATTACK;
        character->state = -1;
        map_character_update(character);
        weapon = 0;
        for (i = 0; i < character->items_num; ++i)
            if (object_is_weapon(character->items[i])
                && character->item_used[i]) {
                weapon = object_weapon_class(character->items[i]);
                break;
            }
        character->weapon_shape_id =
            shape_get_weapon_shape_id(character->shape_id, weapon);
    }
}



/*
  set characters moving direction
*/

void
character_move(Character *character, int direction)
{
    if (character->action == CHARACTER_STAY) {
        character->x /= 4;
        character->x *= 4;
        character->y /= 4;
        character->y *= 4;
        character->direction = direction;
        character->action = direction;
        map_character_update(character);
    }
}



void
character_move_left(Character *character)
{
    character_move(character, CHARACTER_LEFT);
}

void
character_move_right(Character *character)
{
    character_move(character, CHARACTER_RIGHT);
}

void
character_move_up(Character *character)
{
    character_move(character, CHARACTER_UP);
}

void
character_move_down(Character *character)
{
    character_move(character, CHARACTER_DOWN);
}



/*
  next characters frame
*/

void
character_frame(Character *character)
{

    switch (character->action) {

        case CHARACTER_STAY:
            return;

        case CHARACTER_ATTACK:
            character->state++;
            if (character->weapon_shape_id < 0) {
                if (character->state >= 2) {
                    character->state = 0;
                    character->action = CHARACTER_STAY;
                }
                return;
            }


            if (character->state / 2 >=
                shape_get(character->weapon_shape_id +
                          character->direction)->pixmaps_num) {
                character->state = 0;
                character->action = CHARACTER_STAY;
            }
            return;

        case CHARACTER_LEFT:
            character->x -= 2;
            map_character_update(character);
            if (character->x % 4 == 0) {
                character->state++;
                character->action = CHARACTER_STAY;
            }
            break;

        case CHARACTER_RIGHT:
            character->x += 2;
            map_character_update(character);
            if (character->x % 4 == 0) {
                character->state++;
                character->action = CHARACTER_STAY;
            }
            break;

        case CHARACTER_UP:
            character->y -= 2;
            map_character_update(character);
            if (character->y % 4 == 0) {
                character->state++;
                character->action = CHARACTER_STAY;
            }
            break;

        case CHARACTER_DOWN:
            character->y += 2;
            map_character_update(character);
            if (character->y % 4 == 0) {
                character->state++;
                character->action = CHARACTER_STAY;
            }
            break;

    }

    if (character->state == 4)
        character->state = 0;


}



/*
  discards character item
*/

void
character_discard_item(Character *character, int index)
{
    if (index >= 0 && index <= character->items_num) {
        for (; index + 1 < character->items_num; ++index) {
            character->items[index] = character->items[index + 1];
            character->item_used[index] = character->item_used[index + 1];
        }
        character->items_num--;
    }
}

/*
  adds character item
*/

int
character_add_item(Character *character, int item)
{

    if (!object_is_item(item))
        return 0;
    if (character->items_num == 10)
        return 0;

    character->items[character->items_num] = item;
    character->item_used[character->items_num] = 0;

    if (object_is_ring(item))
        character->ring_mode = 1;

    character->items_num++;

    return 1;
}


/*
  adds character spell
*/

int
character_add_spell(Character *character, int spell)
{

    int i;

    if (!object_is_spell(spell))
        return 0;
    if (character->spells_num == 10)
        return 0;

    if (object_name(spell)[0] != '!') {
        for (i = 0; i < character->spells_num; ++i)
            if (character->spells[i] == spell)
                return 0;
    }

    character->spells[character->spells_num] = spell;

    character->spells_num++;

    return 1;
}



/*
  adds character skill
*/

int
character_add_skill(Character *character, int skill)
{

    int i;

    if (!object_is_skill(skill))
        return 0;
    if (character->skills_num == 10)
        return 0;

    for (i = 0; i < character->skills_num; ++i)
        if (character->skills[i] == skill)
            return 0;

    character->skills[character->skills_num] = skill;

    character->skills_num++;

    return 1;
}





/*
  perform COMMAND_NPC_INIT
*/

void
character_command_npc_init(Character *character, int type, int value)
{

    /* we set characters parameter at position <type> in original lotr
       npc structure */

    switch (type) {
        case 0x57:             /* text */
            character->texts[0] = value;
            character->pythontexts = 0;
            break;

        case 0x2c:             /* spell */
            if (character->spells_num < 10)
                character->spells[character->spells_num++] = value;
            break;

        case 0x0a:             /* direction */
            character->direction = value;
            map_character_update(character);
            break;

        case 0x06:             /* shape */
            if (value != 0xff) {
                character_set_shape(character, character->id, value);
            }
            game_draw_map();
            break;

        case 0x0e:             /* horse shape */
            character->horse_shape_id = value;
            game_draw_map();
            break;

        case 0x50:             /* portrait */
            character->portrait = value;
            break;

        default:
            fprintf(stderr, "lotr: unknown NPC_INIT type %02x\n", type);
    }
}




/*
  character don't use object anymore
*/

int
character_unuse(Character *character, int index)
{
    int item, i;

    item = character->items[index];

    if (object_is_armour(item)) {
        for (i = 0; i < character->items_num; ++i)
            if (object_is_armour(character->items[i]))
                character->item_used[i] = 0;

        return 1;
    }


    if (object_is_shield(item)) {
        for (i = 0; i < character->items_num; ++i)
            if (object_is_shield(character->items[i]))
                character->item_used[i] = 0;

        return 1;
    }


    if (object_is_weapon(item)) {
        for (i = 0; i < character->items_num; ++i)
            if (object_is_weapon(character->items[i]))
                character->item_used[i] = 0;

        return 1;
    }

    return 0;

}


/*
  let character use object
*/

void
character_use(Character *character, int index)
{

    int item, i;

    item = character->items[index];

    /* weak characters can not wield heavy weapons */
    if (object_is_weapon(item)
        && object_weapon_weight(item) > character->str)
        return;


    if (character->item_used[index]) {
        character->item_used[index] = 0;
        if (object_is_ring(item))
            character->ring_mode = 1;
        character_unuse(character, index);
        if (item == OBJECT_TORCH)       /* torch */
            game_check_light();
    } else {
        if (object_is_ring(item)) {
            for (i = 0; i < character->items_num; ++i)
                if (object_is_ring(character->items[i]))
                    character->item_used[i] = 0;

            character->item_used[index] = 1;
            character->ring_mode = 2;
        }

        if (character_unuse(character, index))
            character->item_used[index] = 1;

        if (item == OBJECT_TORCH)       /* torch */
            map_set_light(1);

        if (object_food_value(item) > 0) {
            character_discard_item(character, index);
            if (map_get_frame() - character->last_eat > 0x800) {
                character->last_eat = map_get_frame();
                character->life += object_food_value(item);
                if (character->life > character->end)
                    character->life = character->end;
            }
        }
    }
}




/*
  get ringbearer
*/

Character *
character_get_ringbearer()
{
    int i;
    for (i = 0; i < CHARACTERS_NUM; ++i)
        if (lotr_characters[i] && lotr_characters[i]->ring_mode)
            return lotr_characters[i];
    return 0;
}


/*
  set party with given identifier
*/

void
character_set_party(int id)
{
    int size, i;
    Character *party[11];

    for (i = 0, size = 0; size < 10 && i < CHARACTERS_NUM; ++i)
        if (lotr_characters[i] != NULL && lotr_characters[i]->party_id == id)
            party[size++] = lotr_characters[i];

    game_set_party_characters(party, size);

}


/*
  get CHARACTER_TALK_LEN
*/
int
character_get_talk_len(void)
{
    return CHARACTER_TALK_LEN;
}

/*
  check whether a character is using bow
 */
int
character_using_bow(Character *character)
{
    int i;

    for (i = 0; i < character->items_num; ++i) {
        if (character->item_used[i] &&
            object_weapon_class(character->items[i]) == 8)
        {
            return 1;
        }
    }

    return 0;
}

