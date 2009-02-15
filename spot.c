/****************************************************************************

    spot.c
    game command spots handling


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
#include "cartoon.h"
#include "combat.h"
#include "character.h"
#include "game.h"
#include "gui.h"
#include "lord_sdl.h"
#include "map.h"
#include "object.h"
#include "spot.h"
#include "shape.h"
#include "sound.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef CD_VERSION
#include "av.h"
#endif


#define IGNORE_UNKNOWN 0

#define COMMAND_NPC_RECRUIT     0x00
#define COMMAND_NPC_DISMISS     0x1d
#define COMMAND_NPC_INIT        0x01
#define COMMAND_NPC_QUESTIONS   0x1c
#define COMMAND_NPC_SET_NAME    0x36
#define COMMAND_NPC_MOVE        0x2a
#define COMMAND_NPC_DELETE      0x2c
#define COMMAND_NPC_ENEMY       0x11
#define COMMAND_NPC_TURN_TO_ME  0x2d
#define COMMAND_NPC_CHANGE_STAT 0x17

#define COMMAND_IF_RANDOM       0x25

#define COMMAND_IF_DAY          0x08

#define COMMAND_IF_REGISTER     0x0a
#define COMMAND_REGISTER_OFF    0x0b
#define COMMAND_REGISTER_ON     0x20

#define COMMAND_IF_OBJECTS_HERE 0x06
#define COMMAND_IF_OBJECTS_AT_LEAST 0x07
#define COMMAND_OBJECTS_HERE    0x10
#define COMMAND_OBJECT_INC      0x12
#define COMMAND_OBJECT_DEC      0x0d
#define COMMAND_OBJECTS_TO_BUY  0x1f
#define COMMAND_SET_OBJECT      0x21

#define COMMAND_SKILLS_HERE     0x37

#define COMMAND_IF_PARTY        0x05
#define COMMAND_IF_DIRECTION    0x09

#define COMMAND_IF_GAME_REG     0x33
#define COMMAND_GAME_REG_ON     0x35


#define COMMAND_FALSE_THEN      0x13
#define COMMAND_TRUE_THEN       0x14

#define COMMAND_ELSE            0x0e
#define COMMAND_ENDIF           0x0f

#define COMMAND_GOTO            0x15

#define COMMAND_DISABLE_SPOT    0x16
#define COMMAND_TEXT            0x1a
#define COMMAND_TEXT_PAR        0x18
#define COMMAND_QUESTION        0x1b
#define COMMAND_QUESTION_PAR    0x29
#define COMMAND_CARTOON         0x03
#define COMMAND_PLAY_AV         0x41
#define COMMAND_SOUND           0x23
#define COMMAND_END             0x24
#define COMMAND_TELEPORT        0x26
#define COMMAND_ACTION          0x27
#define COMMAND_DIE             0x28
#define COMMAND_SET_SILVER      0x3e
#define COMMAND_EXIT_BUILDING   0x46

#define COMMAND_SET_TIMER       0x22

#define COMMAND_SET_TMP_LEADER     0x3c
#define COMMAND_DISMISS_TMP_LEADER 0x3d

#define COMMAND_MAP_NORMAL      0x1e
#define COMMAND_MAP_FOG         0x0c
#define COMMAND_MAP_DARK        0x39
#define COMMAND_MAP_LIGHT       0x3a
#define COMMAND_IF_MAP_DARK     0x3b

#define COMMAND_COMBAT          0x38

#define COMMAND_CAN_NOT_MOVE    0x31
#define COMMAND_CAN_MOVE        0x30

#ifdef TTT
#define COMMAND_ACTIVATE_PARTY  0x42
#else
#define COMMAND_UNKNOWN42	0x42
#endif

#define COMMAND_02              0x02
#define COMMAND_04              0x04
#ifdef TTT
#define COMMAND_43              0x43
#else
#define COMMAND_END_OF_GAME     0x43
#endif
#define COMMAND_47              0x47

#define COMMAND_UNKNOWN1        0x32
#define COMMAND_UNKNOWN6        0x19    /* proceed frames */
#define COMMAND_UNKNOWN7        0x4e
#define COMMAND_UNKNOWN9        0x44
#define COMMAND_UNKNOWNa        0x2e
#define COMMAND_UNKNOWNb        0x2f
#define COMMAND_UNKNOWNc        0x49
#define COMMAND_UNKNOWNd        0x34
#define COMMAND_UNKNOWNe        0x4b
#define COMMAND_UNKNOWNf        0x4c
#define COMMAND_UNKNOWNg        0x48
#define COMMAND_UNKNOWNi        0x4a
#define COMMAND_UNKNOWNl        0xff
#define COMMAND_UNKNOWNm        0x3f


Uint8 *spot_objects;
Uint8 *buy_objects;


#ifdef TTT
char *spot_chapter_texts[15] = {
    "Meanwhile Frodo and Sam are descending the cliffs around the Dead Marshes.",
    "While others face danger, far away, the Ringbearer treks through an even more perilous country.",
    "The Ringbearer is entering the greatest danger he has faced so far -- Morgul Vale, valley of the Witch-King.",
    "Having survived the perils of Minas Morgul, the Ringbearer prepares to enter the pass of Cirith Ungol, the only way to Mordor.",
    "West of the Great River, south of Fangorn, the search continues for the members of the Fellowship who were captured by the orcs.",
    "Meanwhile, members of the Fellowship search for the messenger orcs of Isengard before they can deliver news to their master.",
    "The Fellowship heads to Edoras, the chief city of Rohan, where all is not well...",
    "Theoden is cured, but a desperate search must take place before the Rohirrim heads to battle.",
    "All eyes are drawn to the Rohirrim fortress of Helm's Deep, where Saruman makes his final assault on the forces of Rohan.",
    "Having bested Saruman at Helm's Deep, the Fellowship marches to parley with Saruman at his fortress of Isengard.",
    "Fleeing from the orcs into the forest of Fangorn, the newly freed members of the Fellowship are about to have a very strange meeting...",
    "Meanwhile, in the forest of Fangorn, members of the Fellowship seek to bring word from Skinbark and Leaflock to the Entmoot.",
    "Treebeard and the Ents of Fangorn are marching toward Saruman's terrible fortress of Isengard.",
    "As the orcs converge on Helm's Deep, one of your members searches for the lost armies of Rohan to bolster its defenses.",
    "Meanwhile, a desperate search is taking place, as the Fellowship looks for help in defeating the legions of Saruman."
};
#endif


#ifdef CD_VERSION

int
cddir_to_dir(int dir)
{
    switch (dir) {
    case CD_NORTH:
        return CHARACTER_UP;
    case CD_EAST:
        return CHARACTER_RIGHT;
    case CD_SOUTH:
        return CHARACTER_DOWN;
    case CD_WEST:
        return CHARACTER_LEFT;
    case 0xff:
        return 0xff;
    }
    /* error return something sensible */
    return CHARACTER_UP;
}

int
dir_to_cddir(int dir)
{
    switch (dir) {
    case CHARACTER_UP:
        return CD_NORTH;
    case CHARACTER_RIGHT:
        return CD_EAST;
    case CHARACTER_DOWN:
        return CD_SOUTH;
    case CHARACTER_LEFT:
        return CD_WEST;
    case 0xff:
        return 0xff;
    }
    /* error return something sensible */
    return CD_NORTH;
}

#endif



/*
  read int
*/

inline int
readint(Uint8 * data)
{
    return data[0] + 0x100 * data[1];
}


/*
  valid letter in a question
*/

int
spot_question_letter(Uint8 c)
{
    return (c >= 'A' && c <= 'Z') || c == ' ' || c == '-' || c == '\'';
}


/*
  init spot commands
*/


void
spot_init_commands(CommandSpot * spot)
{
    int i = 0, j, n = 0;
    int unknown = 0;
    int level = 0;
    int next_level = 0;
#ifdef TTT
    int negative_level = 0;
#endif

    i = spot->headersize;

    /* we have not decoded DEMO spots */
    if (DEMO) {
        spot->commands_num = 0;
        return;
    }

    while (i < spot->data_size && n < SPOT_MAX_COMMANDS) {
        spot->command_start[n] = i;
        switch (spot->data[i]) {

        case COMMAND_UNKNOWNl:
            i = spot->data_size;
            break;

#ifdef TTT
        case COMMAND_43:
            spot->building =
                map_get_building(spot->map, readint(spot->data + i + 1),
                                 readint(spot->data + i + 3));
            spot->floor = spot->data[i + 7];
            i += 8;
            break;
#endif

        case COMMAND_UNKNOWN9:
            i += 6;
            break;

        case COMMAND_UNKNOWNm:
            i += 5;
            break;

        case COMMAND_UNKNOWN6:
            i += 3;
            break;

        case COMMAND_UNKNOWNd:
        case COMMAND_UNKNOWNb:
        case COMMAND_UNKNOWNc:
        case COMMAND_UNKNOWNg:
            i += 2;
            break;

        case COMMAND_UNKNOWN1:
        case COMMAND_UNKNOWN7:
        case COMMAND_UNKNOWNa:
        case COMMAND_UNKNOWNe:
        case COMMAND_UNKNOWNf:
        case COMMAND_UNKNOWNi:
            i += 1;
            break;

        case COMMAND_NPC_ENEMY:
            if (spot->data[i + 5])
                i += 6 + spot->data[i + 4] * 4;
            else {
                spot->data[i] = -1;
                goto unknown_command;
            }
            break;

        case COMMAND_TELEPORT:
            i += 9;
            break;

        case COMMAND_NPC_MOVE:
            i += 7;
            break;

        case COMMAND_NPC_CHANGE_STAT:
            i += 6;
            break;

        case COMMAND_IF_PARTY:
        case COMMAND_02:
            i += 5;
            break;

        case COMMAND_NPC_INIT:
#ifndef TTT
            i += 4;
#else
            if (spot->data[i + 2] == 0)
                i += 5;
            else
                i += 4;
#endif
            break;


        case COMMAND_IF_OBJECTS_HERE:
        case COMMAND_IF_OBJECTS_AT_LEAST:
        case COMMAND_04:
            i += 4;
            break;

        case COMMAND_OBJECTS_HERE:
        case COMMAND_SET_OBJECT:
        case COMMAND_SKILLS_HERE:
        case COMMAND_GOTO:
        case COMMAND_SET_TIMER:
        case COMMAND_NPC_DISMISS:
            i += 3;
            break;

#ifdef TTT
        case COMMAND_ACTIVATE_PARTY:
            i += 3;
            break;
#else
        case COMMAND_UNKNOWN42:
            i += 1;
            break;
#endif

        case COMMAND_SET_SILVER:
#ifndef TTT
            i += 3;
#else
            i += 2;
#endif
            break;

        case COMMAND_NPC_RECRUIT:
#ifndef TTT
            i += 2;
#else
            i += 3;
#endif
            break;

        case COMMAND_DISABLE_SPOT:
        case COMMAND_TEXT:
        case COMMAND_TEXT_PAR:
        case COMMAND_QUESTION:
        case COMMAND_QUESTION_PAR:
        case COMMAND_IF_RANDOM:
        case COMMAND_IF_REGISTER:
        case COMMAND_REGISTER_OFF:
        case COMMAND_REGISTER_ON:
        case COMMAND_IF_GAME_REG:
        case COMMAND_GAME_REG_ON:
        case COMMAND_NPC_TURN_TO_ME:
        case COMMAND_NPC_DELETE:
        case COMMAND_SOUND:
        case COMMAND_CARTOON:
        case COMMAND_PLAY_AV:
        case COMMAND_OBJECT_INC:
        case COMMAND_OBJECT_DEC:
        case COMMAND_SET_TMP_LEADER:
        case COMMAND_MAP_FOG:
            i += 2;
            break;

        case COMMAND_IF_DIRECTION:
            ++i;
            //FIXME strange hack
            if (i < spot->data_size && spot->data[i] != 0x13
                && spot->data[i] != 0x14)
                ++i;
            break;

        case COMMAND_DIE:
        case COMMAND_END:
        case COMMAND_IF_DAY:
        case COMMAND_DISMISS_TMP_LEADER:
        case COMMAND_MAP_NORMAL:
        case COMMAND_MAP_DARK:
        case COMMAND_MAP_LIGHT:
        case COMMAND_IF_MAP_DARK:
        case COMMAND_COMBAT:
        case COMMAND_CAN_NOT_MOVE:
        case COMMAND_CAN_MOVE:
        case COMMAND_EXIT_BUILDING:
        case COMMAND_47:
#ifndef TTT
        case COMMAND_END_OF_GAME:
#endif
            i += 1;
            break;


        case COMMAND_TRUE_THEN:
        case COMMAND_FALSE_THEN:
            next_level = level + 1;
            i += 1;
            break;

        case COMMAND_ELSE:
            next_level = level;
            level--;
            i += 1;
            break;

        case COMMAND_ENDIF:
            level--;
            next_level = level;
            i += 1;
            break;

        case COMMAND_NPC_QUESTIONS:
            i += 2;
            while (spot->data[i] != 0xff) {
                j = 0;
                while (spot_question_letter(spot->data[i]) && j < 20) {
                    ++i;
                    ++j;
                }
                if (spot->data[i] == 0)
                    ++i;
                else if (spot->data[i] == 0xff)
                    --i;
                ++i;
            }
            ++i;
            break;

        case COMMAND_NPC_SET_NAME:
            i += 2;
            while (spot->data[i])
                ++i;
            ++i;
            break;

        case COMMAND_OBJECTS_TO_BUY:
            i += 1;
            while (spot->data[i] != 0xff && i < spot->data_size - 1)
                ++i;
            ++i;
            break;

        case COMMAND_ACTION:
            j = i;
            i += 2 + 5 * spot->data[i + 1];

            while (j + 5 < i &&
                   (i > spot->data_size ||
                    readint(spot->data + i - 2) < spot->label_start ||
                    readint(spot->data + i - 2) >=
                    spot->label_start + spot->data_size)) {
                i -= 5;
            }

            if (i != j + 2 + 5 * spot->data[j + 1]) {
                fprintf(stderr,
                        "lord: WARNING: action command size changed from %d to %d\n",
                        spot->data[j + 1], (i - j - 2) / 5);
                spot->data[j + 1] = (i - j - 2) / 5;
            }
            break;

        default:
          unknown_command:
            unknown = 1;
            i = spot->data_size;
        }

        if (level < 0) {
            spot->commands_num = n;
            spot->not_parsed = 1;
            level = 0;
#ifndef TTT
            spot_print(spot);
            fprintf(stderr, "lord: error: negative level\n");
            exit(1);
#else
            negative_level = 1;
#endif
        }

        spot->command_level[n] = level;
        level = next_level;

        ++n;
        if (unknown)
            break;
    }


    spot->commands_num = n;
    spot->not_parsed = unknown;

#ifdef TTT
    if (negative_level) {
        spot_print(spot);
        fprintf(stderr, "lord: error: negative level\n");
        return;
    }
#endif

    if (i < spot->data_size) {
        fprintf(stderr, "lord: too many commands in the spot\n");
        spot->not_parsed = 1;
        spot_print(spot);
#ifndef TTT
        exit(1);
#else
        return;
#endif
    }

    if (!unknown && i > spot->data_size) {
        if (spot->commands_num)
            spot->commands_num--;
        spot->not_parsed = 1;
        spot_print(spot);
        fprintf(stderr, "lord: wrongly parsed spot i=%04x, size=%04x\n", i,
                spot->data_size);
        exit(1);
    }

    if (!unknown && level != 0) {
        spot->not_parsed = 1;
        spot_print(spot);
        fprintf(stderr, "lord: spot command do not end at level 0\n");
        // exit(1);
    }

    if (unknown) {
        spot->not_parsed = 1;
        spot_print(spot);
        fprintf(stderr, "lord: spot was not parsed completely\n");
#ifndef CD_VERSION
        //TODO remove CDHACK
        exit(1);
#endif
    }


#ifdef DEBUG
    spot_print(spot);
#endif


}



/*
  parse command spots
*/

void
spot_parse(Uint8 * data, int size, CommandSpot * spots[], int *spots_num)
{
    int s;
    int n;
    int i;
    CommandSpot *spot;


#ifndef TTT
#ifndef CD_VERSION
    if (size <= 0x100)
        goto corrupted;
    i = 0x100;
#else
    if (size <= 0x300)
        goto corrupted;
    i = 0x300;
#endif
#else
    if (size <= 0x300)
        goto corrupted;
    i = 0x300;
#endif
    n = 0;

    while (i < size) {
        s = readint(data + i);
        if (s == 0xffff)
            break;
        if (i + s > size)
            goto corrupted;

        spot = lordmalloc(sizeof(CommandSpot));

        spot->headersize = readint(data + i + 2);
        if (spot->headersize > s)
            goto corrupted;

        spot->pythonspot = 0;

        spot->map = data[i + 4];
        spot->building = -1;

        spot->id = n;
        spot->flag = data[n];

        spot->data = lordmalloc(s);
        memcpy(spot->data, data + i, s);
        spot->data_size = s;

        spot->label_start = i;
        spot_init_commands(spot);

        spots[n++] = spot;

        i += s;
    }

    *spots_num = n;

    return;

  corrupted:
    fprintf(stderr, "lord: corrupted game description data\n");
    exit(1);

}





/*
  free command spot
*/

void
spot_free(CommandSpot * spot)
{
    free(spot->data);
    free(spot);
}




/*
  return character name
  (including special characters like leader/ ringbearer, etc...)
*/


char *
spot_character_name(int id)
{
    if (id >= 0 && id < 0xf0) {
        if (character_exists(id))
            return character_get(id)->name;
        else
            return "null";
    }

    if (id == 0xf0)
        return "all";

    if (id == 0xf4)
        return "leader";

    if (id == 0xf7)
        return "Ringbearer";

    if (id == 0xfa || id == 0xfb)
        return "anybody except leader";

    if (id == 0xff)
        return "anybody";

    return "???";
}




/*
  return character
  (including special characters like leader/ ringbearer, etc...)
*/


Character *
spot_character_get(int id)
{

    Character *party[11];
    int party_size;
    int i;

    if (id >= 0 && id < 0xf0) {
        if (character_exists(id))
            return character_get(id);
        else
            return NULL;
    }

    if (id == 0xf0) {
        fprintf(stderr,
                "lord: WARNING - spot_character_get got parameter 0xf0 (whole party) this should not happen!");
        return NULL;
    }

    if (id == 0xf4) {
        return game_get_leader();
    }

    if (id == 0xf7) {
        party_size = game_get_party_characters(party);
        for (i = 0; i < party_size; ++i)
            if (party[i]->ring_mode != 0)
                return party[i];
        return character_get_ringbearer();
    }

    if (id == 0xfa || id == 0xfb) {
        party_size = game_get_party_characters(party);
        for (i = 0; i < party_size; ++i)
            if (party[i] != game_get_leader() && party[i]->map == game_get_leader()->map)       /*needed for Barrow Downs */
                return party[i];
        return NULL;
    }

    return NULL;


}




/*
  print command spot
*/

void
spot_print(CommandSpot * spot)
{

    int i, j, jj, k, endcom;
    char question_key[21];

    if (spot == NULL)
        return;

    printf("\n\n%02x:\n", spot->id);

    i = 5;
    while (i + 6 < spot->headersize) {
        printf("   x=%x y=%x w=%x h=%x\n",
               readint(spot->data + i), readint(spot->data + i + 2),
               spot->data[i + 4], spot->data[i + 5]);
        i += 6;
    }

    printf("header: ");
    for (i = 0; i < spot->headersize; ++i)
        printf("%02x ", spot->data[i]);
    printf("\n");

    for (k = 0; k < spot->commands_num; ++k) {

        i = spot->command_start[k];

        printf("%04x: ", i + spot->label_start);
        for (j = 0; j < spot->command_level[k] * 4; ++j)
            putchar(' ');

        switch (spot->data[i]) {

        case COMMAND_UNKNOWN1:
        case COMMAND_UNKNOWN6:
        case COMMAND_UNKNOWN7:
        case COMMAND_UNKNOWN9:
        case COMMAND_UNKNOWNa:
        case COMMAND_UNKNOWNb:
        case COMMAND_UNKNOWNc:
        case COMMAND_UNKNOWNd:
        case COMMAND_UNKNOWNe:
        case COMMAND_UNKNOWNf:
        case COMMAND_UNKNOWNg:
        case COMMAND_UNKNOWNi:
        case COMMAND_UNKNOWNm:
#ifndef TTT
        case COMMAND_UNKNOWN42:
#endif

            if (k + 1 < spot->commands_num)
                endcom = spot->command_start[k + 1];
            else
                endcom = spot->data_size;

            printf("UNKNOWN: ");
            for (j = i; j < endcom; ++j)
                printf("%02x ", spot->data[j]);


            break;

        case COMMAND_NPC_INIT:
            printf("NPC_INIT: %02x (%s), %02x, %02x ", spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]),
                   spot->data[i + 2], spot->data[i + 3]);
            if (spot->data[i + 2] == 0x57)
                printf("\"%s\"", game_get_text(spot->data[i + 3]));
            else
                printf("(%s)", object_name(spot->data[i + 3]));

            break;

        case COMMAND_NPC_QUESTIONS:
            printf("NPC_QUESTIONS: %02x (%s)", spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]));

            j = i + 2;
            while (spot->data[j] != 0xff) {
                printf("\n         -- ");
                jj = 0;
                while (spot_question_letter(spot->data[j]) && jj < 20) {
                    ++j;
                    ++jj;
                }

                strncpy(question_key, (char *) spot->data + j - jj, jj);
                question_key[jj] = 0;

                if (spot->data[j] == 0)
                    ++j;
                else if (spot->data[j] == 0xff)
                    --j;
                printf("%s \"%s\"", question_key,
                       game_get_text(spot->data[j]));
                ++j;
            }

            break;

        case COMMAND_NPC_SET_NAME:
            printf("NPC_SET_NAME: %02x (%s), %s", spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]),
                   spot->data + i + 2);
            break;

        case COMMAND_NPC_TURN_TO_ME:
            printf("NPC_TURN_TO_ME: %02x (%s)", spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]));
            break;

        case COMMAND_NPC_RECRUIT:
            printf("NPC_RECRUIT: %02x (%s)", spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]));
#ifdef TTT
            printf(", %02x", spot->data[i + 2]);
#endif
            break;

        case COMMAND_NPC_DISMISS:
            printf("NPC_DISMISS: %02x (%s), %02x", spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]), spot->data[i + 2]);
            break;

        case COMMAND_SET_TMP_LEADER:
            printf("SET_TMP_LEADER: %02x (%s)", spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]));
            break;

        case COMMAND_DISMISS_TMP_LEADER:
            printf("DISMISS_TMP_LEADER");
            break;

        case COMMAND_NPC_MOVE:
            printf("NPC_MOVE: %02x (%s), %02x, %04x, %04x",
                   spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]),
                   spot->data[i + 2], readint(spot->data + i + 3),
                   readint(spot->data + i + 5));
            break;

        case COMMAND_NPC_ENEMY:
            printf
                ("NPC_ENEMY: %02x (%s), relative=%02x, direction=%02x, unknown=%d%%, n=%d",
                 spot->data[i + 5], spot_character_name(spot->data[i + 5]),
                 spot->data[i + 1], spot->data[i + 2], spot->data[i + 3],
                 spot->data[i + 4]);

            for (j = 0; j < spot->data[i + 4]; ++j)
                printf("\n                   x=%04x, y=%04x",
                       readint(spot->data + i + 4 * j + 6),
                       readint(spot->data + i + 4 * j + 8));

            break;

        case COMMAND_NPC_CHANGE_STAT:
            printf
                ("NPC_CHANGE_STAT: npc=%02x, stat=%02x, +-=%02x, value=%02x, %02x",
                 spot->data[i + 1], spot->data[i + 2], spot->data[i + 3],
                 spot->data[i + 4], spot->data[i + 5]);
            break;

        case COMMAND_NPC_DELETE:
            printf("NPC_DELETE: %02x (%s)", spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]));
            break;

        case COMMAND_MAP_FOG:
            printf("MAP_FOG: %02x", spot->data[i + 1]);
            break;

        case COMMAND_MAP_NORMAL:
            printf("MAP_NORMAL");
            break;

        case COMMAND_MAP_DARK:
            printf("MAP_DARK");
            break;

        case COMMAND_MAP_LIGHT:
            printf("MAP_LIGHT");
            break;

        case COMMAND_IF_MAP_DARK:
            printf("IF_MAP_DARK");
            break;

        case COMMAND_GOTO:
            printf("GOTO: %04x", readint(spot->data + i + 1));
            break;

        case COMMAND_DISABLE_SPOT:
            printf("DISABLE_SPOT: %02x", spot->data[i + 1]);
            break;

        case COMMAND_TEXT:
            printf("TEXT: \"%s\"", game_get_text(spot->data[i + 1]));
            break;

        case COMMAND_QUESTION:
            printf("QUESTION: \"%s\"", game_get_text(spot->data[i + 1]));
            break;

        case COMMAND_TEXT_PAR:
            printf("TEXT_PAR: %d", spot->data[i + 1]);
            break;

        case COMMAND_QUESTION_PAR:
            printf("QUESTION_PAR: %d", spot->data[i + 1]);
            break;

        case COMMAND_ACTION:
            printf("ACTION: %02x", spot->data[i + 1]);
            for (j = 0; j < spot->data[i + 1]; ++j) {
                printf
                    ("\n           -- %02x, %02x (%s), %02x (%s): goto %04x",
                     spot->data[i + 2 + j * 5], spot->data[i + 3 + j * 5],
                     object_name(spot->data[i + 3 + j * 5]),
                     spot->data[i + 4 + j * 5],
                     spot_character_name(spot->data[i + 4 + j * 5]),
                     readint(spot->data + i + 5 + j * 5));
            }
            break;

        case COMMAND_IF_PARTY:
            printf("IF_PARTY: %02x (%s), %02x, %02x, %02x\n",
                   spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]),
                   spot->data[i + 2], spot->data[i + 3], spot->data[i + 4]);
            break;

        case COMMAND_02:
            printf("COMMAND_02: %02x (%s), %02x, %02x, %02x",
                   spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]),
                   spot->data[i + 2], spot->data[i + 3], spot->data[i + 4]);
            break;

        case COMMAND_04:
            printf("COMMAND_04: %02x (%s), %02x (%s), %02x",
                   spot->data[i + 1],
                   spot_character_name(spot->data[i + 1]),
                   spot->data[i + 2],
                   spot_character_name(spot->data[i + 2]), spot->data[i + 3]);
            break;

#ifdef TTT
        case COMMAND_43:
            printf("COMMAND_43: building=%02x, %02x, %02x, floor=%d\n",
                   spot->building, spot->data[i + 5], spot->data[i + 6],
                   spot->data[i + 7]);
            break;
#endif

        case COMMAND_47:
            printf("COMMAND_47\n");
            break;

        case COMMAND_IF_RANDOM:
            printf("IF_RANDOM: %02x", spot->data[i + 1]);
            break;

        case COMMAND_IF_DIRECTION:
            printf("IF_DIRECTION: %02x", spot->data[i + 1]);
            break;

        case COMMAND_IF_DAY:
            printf("IF_DAY");
            break;

#ifndef TTT
        case COMMAND_END_OF_GAME:
            printf("END_OF_GAME");
            break;
#endif

        case COMMAND_IF_REGISTER:
            printf("IF_REGISTER: %02x", spot->data[i + 1]);
            if (map_get_register(spot->data[i + 1]))
                printf(" (true)");
            else
                printf(" (false)");
            break;

        case COMMAND_REGISTER_OFF:
            printf("REGISTER_OFF: %02x", spot->data[i + 1]);
            break;

        case COMMAND_REGISTER_ON:
            printf("REGISTER_ON: %02x", spot->data[i + 1]);
            break;


        case COMMAND_IF_GAME_REG:
            printf("IF_GAME_REG: %02x", spot->data[i + 1]);
            break;

        case COMMAND_GAME_REG_ON:
            printf("GAME_REG_ON: %02x", spot->data[i + 1]);
            break;

        case COMMAND_TRUE_THEN:
            printf("  TRUE_THEN");
            break;

        case COMMAND_FALSE_THEN:
            printf("  FALSE_THEN");
            break;

        case COMMAND_ELSE:
            printf("  ELSE");
            break;

        case COMMAND_ENDIF:
            printf("  ENDIF");
            break;

        case COMMAND_TELEPORT:
            printf("TELEPORT: %02x, dir=%02x, x=%04x, y=%04x, map=%d, %02x",
                   spot->data[i + 1], spot->data[i + 2],
                   readint(spot->data + i + 3),
                   readint(spot->data + i + 5), spot->data[i + 7],
                   spot->data[i + 8]);
            break;

        case COMMAND_SOUND:
            printf("SOUND: index=%04x", spot->data[i + 1]);
            break;

        case COMMAND_CARTOON:
            printf("CARTOON: cartoon=%d", spot->data[i + 1] + 1);
            break;

        case COMMAND_PLAY_AV:
            printf("PLAY_AV: av=%d", spot->data[i + 1] + 1);
            break;

        case COMMAND_COMBAT:
            printf("COMBAT");
            break;

        case COMMAND_SET_SILVER:
#ifndef TTT
            printf("SET_SILVER: amount=%04x", readint(spot->data + i + 1));
#else
            printf("ADD_SILVER: amount=%02x", spot->data[i + 1]);
#endif
            break;

        case COMMAND_OBJECTS_HERE:
            printf("OBJECTS_HERE: index=%04x", readint(spot->data + i + 1));
            break;

        case COMMAND_OBJECT_INC:
            printf("OBJECT_INC: index=%02x", spot->data[i + 1]);
            break;

        case COMMAND_OBJECT_DEC:
            printf("OBJECT_DEC: index=%02x", spot->data[i + 1]);
            break;

        case COMMAND_SET_OBJECT:
            printf("SET_OBJECT: index=%02x, value=%02x", spot->data[i + 1],
                   spot->data[i + 2]);
            break;

        case COMMAND_IF_OBJECTS_HERE:
            printf("IF_OBJECTS_HERE: index=%04x, %02x",
                   readint(spot->data + i + 1), spot->data[i + 3]);
            break;

        case COMMAND_IF_OBJECTS_AT_LEAST:
            printf("IF_OBJECTS_AT_LEAST: index=%04x, %02x",
                   readint(spot->data + i + 1), spot->data[i + 3]);
            break;

        case COMMAND_OBJECTS_TO_BUY:
            printf("OBJECTS_TO_BUY: ");
            for (j = i + 1; spot->data[j] != 0xff; ++j)
                printf("(%s), ", object_name(spot->data[j]));
            break;

        case COMMAND_SKILLS_HERE:
            printf("SKILLS_HERE: index=%04x", readint(spot->data + i + 1));
            break;

        case COMMAND_END:
            printf("END");
            break;

        case COMMAND_DIE:
            printf("DIE");
            break;

#ifdef TTT
        case COMMAND_ACTIVATE_PARTY:
            printf("ACTIVATE_PARTY %d, text=\"%s\"", spot->data[i + 1],
                   spot_chapter_texts[spot->data[i + 2]]);
            break;
#endif

        case COMMAND_CAN_NOT_MOVE:
            printf("CAN_NOT_MOVE");
            break;

        case COMMAND_CAN_MOVE:
            printf("CAN_MOVE");
            break;

        case COMMAND_EXIT_BUILDING:
            printf("EXIT_BUILDING");
            break;

        case COMMAND_SET_TIMER:
            printf("SET_TIMER: %04x", readint(spot->data + i + 1));
            break;

        default:
            printf("UNKNOWN: ");
            for (; i < spot->data_size; ++i)
                printf("%02x ", spot->data[i]);

            spot->not_parsed = 1;
        }

        printf("\n");

    }

    if (spot->not_parsed) {
        printf("\nCOMPLETE LIST: ");
        for (i = 0; i < spot->data_size; ++i)
            printf("%02x ", spot->data[i]);
    }

    if (spot->pythonspot)
        printf("PYTHON SPOT id=%d\n", spot->id);

    printf("\n");

}



/*
  start running command spot
*/

int
spot_start(CommandSpot * spot)
{
    spot->pos = 0;
    spot_objects = NULL;
    buy_objects = NULL;
#ifdef EXTENDED
    if (spot->pythonspot)
        pythonspot_start(spot->id);
#endif
    return spot_continue(spot);
}


/*
  proceed to the next command on the same level
 */

void
spot_next_on_level(CommandSpot * spot)
{
    int level = spot->command_level[spot->pos];
    spot->pos++;
    while (spot->pos < spot->commands_num &&
           spot->command_level[spot->pos] != level)
        spot->pos++;

    if (!spot->not_parsed && spot->pos >= spot->commands_num) {
        fprintf(stderr, "lord: wrong spot command IF/ELSE/ENDIF nesting\n");
        exit(1);
    }

}



/*
  goto address
 */

void
spot_goto(CommandSpot * spot, int address)
{
    int i;

    for (i = 0; i < spot->commands_num; ++i)
        if (spot->command_start[i] + spot->label_start == address) {
            spot->pos = i;
            return;
        }

    spot_print(spot);
    fprintf(stderr, "lord: could not find address %04x\n", address);
    // exit(1);

}



/*
   change characters stat
*/

void
spot_change_stat(Character * character, int stat, int sign, int value)
{
    if (sign == 0xff)
        value = -value;
    if (sign != 0xff && sign != 1)
        fprintf(stderr, "lord: WARNING unknown change stat sign=%02x\n",
                sign);

    switch (stat) {
    case 0x10:
        character->dex += value;
        break;
    case 0x11:
        character->life += value;
        break;
    case 0x12:
        character->str += value;
        break;
    case 0x13:
        character->will += value;
        break;
    case 0x15:
        character->luck += value;
        break;
    case 0x16:
        character->end += value;
        if (value > 0)
            character->life += value;
        break;
    default:
        fprintf(stderr, "lord: WARNING unknown stat number=%02x\n", stat);
        break;
    }

    if (character->life > character->end)
        character->life = character->end;

    if (character->life <= 0) {
        character->life = 0;
        gui_player_dead(character, 1);
    }

}


/*
   spot if party command
*/

int
spot_if_party(Character * character, int a, int b, int c)
{

    int i;
    int codes[10];

    if (character == NULL) {
        fprintf(stderr, "lord: spot_if_party got NULL character\n");
        return 0;
    }

    switch (a) {

    case 1:                    /* if has skill */
        for (i = 0; i < character->skills_num; ++i)
            if (character->skills[i] == b)
                return 1;
        return 0;

    case 2:                    /* if has item */
        for (i = 0; i < character->items_num; ++i)
            if (character->items[i] == b)
                return 1;
        return 0;

    case 0xa:                  /* if is in party */
        if (b == 0x08)
            return character == game_get_leader();

        if (b == 0x20)
            return game_in_party(character);

        fprintf(stderr, "lord: WARNING unknown IF_PARTY type=%02x %02x\n", a,
                b);
        return 0;


    case 0xb:
        return game_get_party(codes) == b;

    default:
        fprintf(stderr, "lord: WARNING unknown IF_PARTY type=%02x\n", a);
        return 0;
    }

    return 1;
}


/*
  set if result
 */

void
spot_if_result(CommandSpot * spot, int result)
{
#ifdef EXTENDED
    if (spot->pythonspot)
        pythonspot_if_result(result);
#endif
    spot->if_result = result;
}


/*
  continue running command spot
*/

int
spot_continue(CommandSpot * spot)
{
    int i, j, jj, n;
    int x, y;
    int relative;
    Character *character = NULL;
    Character *leader;
    char name[20];

    Character *party[11];
    int party_size;

    if (spot == NULL)
        return 0;

#ifdef EXTENDED
    if (spot->pythonspot)
        return pythonspot_continue(spot->id);
#endif

    party_size = game_get_party_characters(party);

    while (spot->pos < spot->commands_num) {
        i = spot->command_start[spot->pos];

        switch (spot->data[i]) {

        case COMMAND_02:
        case COMMAND_47:
            // FIXME
            spot->pos++;
            break;

#ifdef TTT
        case COMMAND_43:
            spot->pos++;
            spot_if_result(spot, 1);
            break;
#endif

        case COMMAND_UNKNOWN1:
        case COMMAND_UNKNOWN6:
            // FIXME
            spot->pos++;
            break;


        case COMMAND_NPC_INIT:
            spot->pos++;
            if (spot->data[i + 1] == 0xf0) {
                for (j = 0; j < party_size; ++j)
#ifdef CD_VERSION
                    /* cd version uses different direction codes */
                    if (spot->data[i + 2] == 0xa)       /* direction */
                        character_command_npc_init(party[j],
                                                   spot->data[i + 2],
                                                   cddir_to_dir(spot->data[i +
                                                                           3]));
                    else
#endif
                        character_command_npc_init(party[j],
                                                   spot->data[i + 2],
                                                   spot->data[i + 3]);
                break;
            }

            character = spot_character_get(spot->data[i + 1]);

            if (character) {
#ifdef CD_VERSION
                /* cd version uses different direction codes */
                if (spot->data[i + 2] == 0xa)   /* direction */
                    character_command_npc_init(character, spot->data[i + 2],
                                               cddir_to_dir(spot->data
                                                            [i + 3]));
                else
#endif
                    character_command_npc_init(character, spot->data[i + 2],
                                               spot->data[i + 3]);
            }
            break;

        case COMMAND_NPC_SET_NAME:
            spot->pos++;
            character = spot_character_get(spot->data[i + 1]);
            if (character == NULL)
                break;
            strncpy(character->name, (char *) spot->data + i + 2, 19);
            character->name[20] = 0;
            break;

        case COMMAND_NPC_QUESTIONS:
            spot->pos++;
            character = spot_character_get(spot->data[i + 1]);
            character->pythontexts = 0;
            if (character == NULL)
                break;
            j = 1;
            i += 2;
            while (spot->data[i] != 0xff) {
                jj = 0;
                while (spot->data[i] && jj < 10) {
                    character->texts[j++] = spot->data[i++];
                    ++jj;
                }
                if (!spot->data[i])
                    character->texts[j++] = spot->data[i++];
                character->texts[j++] = spot->data[i++];
            }
            character->texts[j++] = spot->data[i++];
            break;

        case COMMAND_NPC_TURN_TO_ME:
            spot->pos++;
            character = game_get_leader();
            map_character_turn_to(spot->data[i + 1], character->x,
                                  character->y);
            break;

        case COMMAND_NPC_CHANGE_STAT:
            spot->pos++;
            if (spot->data[i + 1] == 0xf0) {
                for (j = 0; j < party_size; ++j)
                    spot_change_stat(party[j], spot->data[i + 2],
                                     spot->data[i + 3], spot->data[i + 4]);
                break;

            }
            character = spot_character_get(spot->data[i + 1]);

            if (character != NULL)
                spot_change_stat(character, spot->data[i + 2],
                                 spot->data[i + 3], spot->data[i + 4]);
            break;

        case COMMAND_NPC_MOVE:
            InputDisable();
            ResetKeyboard();
#ifndef CD_VERSION
            if (map_character_move
                (spot->data[i + 1], readint(spot->data + i + 3),
                 readint(spot->data + i + 5), spot->data[i + 2]))
#else
            if (map_character_move
                (spot->data[i + 1], readint(spot->data + i + 3),
                 readint(spot->data + i + 5),
                 cddir_to_dir(spot->data[i + 2])))
#endif
                return 1;
            else
                spot->pos++;
            InputEnable();
            break;

        case COMMAND_NPC_DELETE:
            spot->pos++;
            map_remove_character(spot->data[i + 1]);
            character = character_get(spot->data[i + 1]);
            character->map = -1;
            // FIXME
            // character->actived=1;
            break;

        case COMMAND_NPC_RECRUIT:
            spot->pos++;
            if (spot->data[i + 1] == 0xfa) {
                game_set_follow(1);
                break;
            }
            if (spot->data[i + 1] < 0xf0) {
                character = character_get(spot->data[i + 1]);
                character->party_id = spot->data[i + 2];
                game_recruit(character_get(spot->data[i + 1]), 1);
            }
            break;

        case COMMAND_NPC_DISMISS:
            spot->pos++;
            if (spot->data[i + 1] == 0xfa) {
                game_set_follow(0);
                break;
            }
            character = spot_character_get(spot->data[i + 1]);
            if (spot->data[i + 1] == 0xfb)
                character = NULL;
            if (character != NULL) {
                // FIXME trade items if spot->data[i+2]
                game_dismiss(character);
                character->willing_join = 0;
            }
            break;

        case COMMAND_SET_TMP_LEADER:
            spot->pos++;
            character = spot_character_get(spot->data[i + 1]);
            if (character != NULL)
                game_set_tmp_leader(character);
            break;

        case COMMAND_DISMISS_TMP_LEADER:
            spot->pos++;
            game_dismiss_tmp_leader();
            break;

        case COMMAND_DISABLE_SPOT:
            spot->pos++;
            map_get_spot_number(spot->data[i + 1])->flag = 0;
            break;

        case COMMAND_ACTION:
            return 0;

        case COMMAND_TELEPORT:
            spot->pos++;
            if (spot->data[i + 8] != 0xf0)
                character = spot_character_get(spot->data[i + 8]);
            if (spot->data[i + 8] == 0xf0 || character == game_get_leader()) {
#ifndef CD_VERSION
                game_leader_teleport(spot->data[i + 1],
                                     readint(spot->data + i + 3),
                                     readint(spot->data + i + 5),
                                     spot->data[i + 2], spot->data[i + 7]);
#else
                game_leader_teleport(spot->data[i + 1],
                                     readint(spot->data + i + 3),
                                     readint(spot->data + i + 5),
                                     cddir_to_dir(spot->data[i + 2]),
                                     spot->data[i + 7]);
#endif
                spot_if_result(spot, 1);
                return 1;
            }
            if (character != NULL) {
#ifndef CD_VERSION
                map_character_teleport(character, spot->data[i + 1],
                                       readint(spot->data + i + 3),
                                       readint(spot->data + i + 5),
                                       spot->data[i + 2], spot->data[i + 7]);
#else
                map_character_teleport(character, spot->data[i + 1],
                                       readint(spot->data + i + 3),
                                       readint(spot->data + i + 5),
                                       cddir_to_dir(spot->data[i + 2]),
                                       spot->data[i + 7]);
#endif
                spot_if_result(spot, 1);
                return 1;
            }

            spot_if_result(spot, 0);
            break;

        case COMMAND_REGISTER_ON:
            spot->pos++;
            map_set_register(spot->data[i + 1], 1);
            break;

        case COMMAND_REGISTER_OFF:
            spot->pos++;
            map_set_register(spot->data[i + 1], 0);
            break;

        case COMMAND_IF_REGISTER:
            spot->pos++;
            spot_if_result(spot, map_get_register(spot->data[i + 1]));
            break;

        case COMMAND_IF_GAME_REG:
            spot->pos++;
            spot_if_result(spot, game_get_register(spot->data[i + 1]));
            break;

        case COMMAND_GAME_REG_ON:
            spot->pos++;
            game_set_register(spot->data[i + 1], 1);
            break;

        case COMMAND_IF_PARTY:
            spot->pos++;
            if (spot->data[i + 1] == 0xf0) {
                for (j = 0; j < party_size; ++j)
                    if (spot_if_party
                        (party[j], spot->data[i + 2], spot->data[i + 3],
                         spot->data[i + 4]))
                        break;

                spot_if_result(spot, j < party_size);
                break;
            }

            character = spot_character_get(spot->data[i + 1]);
            spot_if_result(spot,
                           spot_if_party(character, spot->data[i + 2],
                                         spot->data[i + 3],
                                         spot->data[i + 4]));

            break;

        case COMMAND_IF_OBJECTS_HERE:
            spot->pos++;
            j = readint(spot->data + i + 1);
            spot_if_result(spot, map_get_objects()[j] == spot->data[i + 3]);
            break;

        case COMMAND_IF_OBJECTS_AT_LEAST:
            spot->pos++;
            j = readint(spot->data + i + 1);
            spot_if_result(spot, map_get_objects()[j] >= spot->data[i + 3]);
            break;

        case COMMAND_OBJECT_INC:
            spot->pos++;
            j = spot->data[i + 1];
            map_get_objects()[j]++;
            break;

        case COMMAND_OBJECT_DEC:
            spot->pos++;
            j = spot->data[i + 1];
            map_get_objects()[j]--;
            break;

        case COMMAND_SET_OBJECT:
            spot->pos++;
            j = spot->data[i + 1];
            map_get_objects()[j] = spot->data[i + 2];
            break;

        case COMMAND_SKILLS_HERE:
        case COMMAND_OBJECTS_HERE:
            spot->pos++;
            j = readint(spot->data + i + 1);
            spot_objects = map_get_objects() + j;
#ifdef TTT
            if (spot->data[i] == COMMAND_OBJECTS_HERE) {
                gui_message("You see items here you might use.", 0);
                return 1;
            }
#endif
            break;

        case COMMAND_OBJECTS_TO_BUY:
            spot->pos++;
            buy_objects = spot->data + i + 1;
            break;

        case COMMAND_SET_SILVER:
            spot->pos++;
#ifndef TTT
            game_set_silver(readint(spot->data + i + 1));
#else
            game_add_silver(spot->data[i + 1]);
#endif
            break;

        case COMMAND_TRUE_THEN:
            if (!spot->if_result) {
                spot_next_on_level(spot);
                spot->pos++;
            }
            else
                spot->pos++;
            break;

        case COMMAND_IF_DAY:
            spot->pos++;
            spot_if_result(spot, !map_is_night());
            break;

        case COMMAND_IF_RANDOM:
            spot->pos++;
            spot_if_result(spot, lord_rnd(99) < spot->data[i + 1]);
            break;

        case COMMAND_IF_DIRECTION:
            spot->pos++;
#ifndef CD_VERSION
            spot_if_result(spot,
                           game_get_leader()->direction == spot->data[i + 1]);
#else
            spot_if_result(spot,
                           game_get_leader()->direction ==
                           cddir_to_dir(spot->data[i + 1]));
#endif
            break;

        case COMMAND_FALSE_THEN:
            if (spot->if_result) {
                spot_next_on_level(spot);
                spot->pos++;
            }
            else
                spot->pos++;
            break;

        case COMMAND_ENDIF:
            spot->pos++;
            break;

        case COMMAND_ELSE:
            spot_next_on_level(spot);
            break;

        case COMMAND_GOTO:
            spot_goto(spot, readint(spot->data + i + 1));
            break;

        case COMMAND_TEXT:
            gui_message(game_get_text(spot->data[i + 1]), 0);
            spot->pos++;
            return 1;

        case COMMAND_QUESTION:
            gui_question(game_get_text(spot->data[i + 1]));
            spot->pos++;
            return 1;

        case COMMAND_TEXT_PAR:
            gui_paragraph(spot->data[i + 1]);
            spot->pos++;
            return 1;

        case COMMAND_QUESTION_PAR:
            gui_paragraph_question(spot->data[i + 1]);
            spot->pos++;
            return 1;

        case COMMAND_SOUND:
            sound_play(spot->data[i + 1]);
            spot->pos++;
            return 1;

        case COMMAND_SET_TIMER:
            game_set_timer(readint(spot->data + i + 1) + 1);
            spot->pos++;
            return 1;

        case COMMAND_CARTOON:
#if !DEMO
            sprintf(name, "cart%d", spot->data[i + 1] + 1);
            play_music();
#else
            sprintf(name, "democrt%d", spot->data[i + 1] + 1);
#endif
            playcartoon(name);

            if (spot->data[i + 1] + 1 == 11)
                exit(0);        /* You have won */

            ClearScreen();
            gui_set_palette();
            gui_clear();
            map_set_palette();
            shapes_set_palette();

            spot->pos++;
            return 1;

        case COMMAND_PLAY_AV:

#ifdef CD_VERSION

            playavnum(spot->data[i + 1]);

            ClearScreen();
            gui_set_palette();
            gui_clear();
            map_set_palette();
            shapes_set_palette();

            spot->pos++;
            return 1;
#else
            fprintf(stderr, "lord: PLAY_AV command not implemented\n");
            fprintf(stderr, "(recompile with CD_VERSION support)\n");
            spot->pos++;
            break;
#endif


        case COMMAND_MAP_FOG:
            map_set_mode(MAP_FOG, spot->data[i + 1]);
            spot->pos++;
            break;

        case COMMAND_MAP_LIGHT:
        case COMMAND_MAP_NORMAL:
            map_set_mode(MAP_NORMAL, 0);
            spot->pos++;
            break;

        case COMMAND_MAP_DARK:
            map_set_dark();
            game_check_light();
            spot->pos++;
            break;

        case COMMAND_IF_MAP_DARK:
            spot->pos++;
            spot_if_result(spot, map_is_dark());
            break;

        case COMMAND_NPC_ENEMY:
            spot->pos++;

            character = character_get(spot->data[i + 5]);
            relative = spot->data[i + 1];

#ifndef CD_VERSION
            if (spot->data[i + 2] < 5)
                character->direction = spot->data[i + 2];
#else
            if (spot->data[i + 2] < 9)
                character->direction = cddir_to_dir(spot->data[i + 2]);
#endif
            /* this is probably not right
               character->life=character->end*spot->data[i+3]/100;
             */
            character->life = character->end;

            leader = game_get_leader();

            n = spot->data[i + 4];
            for (j = 0; j < n; ++j) {
                x = readint(spot->data + i + 4 * j + 6);
                y = readint(spot->data + i + 4 * j + 8);
                if (relative) {
                    if (x > 0x8000)
                        x = x - 0xffff;
                    if (y > 0x8000)
                        y = y - 0xffff;
                    x += leader->x;
                    y += leader->y;
                    // FIXME do not set enemies to the wall
                }
                character->x = x;
                character->y = y;

                character->map = map_get_id();
                map_add_character(character);
            }
            break;

        case COMMAND_CAN_NOT_MOVE:
            game_set_moving(0);
            spot->pos++;
            break;

        case COMMAND_CAN_MOVE:
            game_set_moving(1);
            spot->pos++;
            break;

        case COMMAND_04:
            // FIXME
            if (spot->data[i + 2] == 0xff && spot->data[i + 3] == 0x4) {
                /* attack me */
                if (spot->data[i + 1])
                    map_attacking_character(spot->data[i + 1]);
            }
            if (spot->data[i + 2] == 0xff && spot->data[i + 3] == 0x10) {
                /* willing to join */
                if (character_exists(spot->data[i + 1]))
                    character_get(spot->data[i + 1])->willing_join = 1;
            }
            if (spot->data[i + 1] == 0xf4 && spot->data[i + 3] == 0x00) {
                /* set leader */
                character = spot_character_get(spot->data[i + 2]);
                if (character != NULL)
                    game_set_leader(character);
            }
            if (spot->data[i + 2] == 0xff && spot->data[i + 3] == 0x20) {
                /* join */
                character = spot_character_get(spot->data[i + 1]);
                if (character != NULL)
                    game_recruit(character, 1);
            }
            spot->pos++;
            break;


        case COMMAND_COMBAT:
            spot->pos++;
            combat_start();
            return 1;


#ifdef TTT
        case COMMAND_EXIT_BUILDING:
            spot->pos++;
            map_exit_building();
            break;
#endif


        case COMMAND_DIE:
            //FIXME write: You have failed, Sauron...
            exit(0);

        case COMMAND_END:
            return 0;

#ifndef TTT
        case COMMAND_END_OF_GAME:
            exit(0);
#endif

#ifdef TTT
        case COMMAND_ACTIVATE_PARTY:
            j = spot->data[i + 2];
            game_party_activate(spot->data[i + 1]);
            gui_book(spot_chapter_texts[j]);
            return 0;
#endif

        default:
            spot_print(spot);
            fprintf(stderr, "lord: unknown spot command %02x\n",
                    spot->data[i]);
            // exit(1);
            spot->pos++;
        }
    }

    return 0;
}



/*
  spot action
*/

int
spot_action(CommandSpot * spot, int type, int what, int on_who)
{
    int i;
    int index;
    int s_type;
    int s_what;
    int s_on_who;

    if (spot == NULL)
        return 0;

#ifdef EXTENDED
    if (spot->pythonspot) {
        if (pythonspot_has_action(spot->id, type, what, on_who)) {
            quit_menu();
            game_continue_spot();
            pythonspot_action(spot->id, type, what, on_who);
            return 1;
        }
        else {
            return 0;
        }
    }
#endif

    if (spot->pos >= spot->commands_num)
        return 0;
    index = spot->command_start[spot->pos];

    if (spot->data[index] != COMMAND_ACTION)
        return 0;

    for (i = 0; i < spot->data[index + 1]; ++i) {
        s_type = spot->data[index + 2 + i * 5];
        s_what = spot->data[index + 2 + i * 5 + 1];
#ifdef CD_VERSION
        if (s_type == SPOT_ACTION_MOVE)
            s_what = cddir_to_dir(s_what);
#endif
        s_on_who = spot->data[index + 2 + i * 5 + 2];

        if ((s_type == 0xff || s_type == type) &&
            (s_what == 0xff || s_what == what) &&
            (s_on_who == 0xff || s_on_who == on_who)) {
            spot_goto(spot, readint(spot->data + index + 2 + i * 5 + 3));

            quit_menu();
            game_continue_spot();

            return 1;
        }
    }


    return 0;
}


/*
  can do an action on this spot
*/

int
spot_has_action(CommandSpot * spot, int type, int what, int on_who)
{
    int i;
    int index;
    int s_type;
    int s_what;
    int s_on_who;

    if (spot == NULL)
        return 0;

#ifdef EXTENDED
    if (spot->pythonspot)
        return pythonspot_has_action(spot->id, type, what, on_who);
#endif

    if (spot->pos > spot->commands_num)
        return 0;
    index = spot->command_start[spot->pos];

    if (spot->data[index] != COMMAND_ACTION)
        return 0;

    for (i = 0; i < spot->data[index + 1]; ++i) {
        s_type = spot->data[index + 2 + i * 5];
        s_what = spot->data[index + 2 + i * 5 + 1];
#ifdef CD_VERSION
        if (s_type == SPOT_ACTION_MOVE)
            s_what = cddir_to_dir(s_what);
#endif
        s_on_who = spot->data[index + 2 + i * 5 + 2];
        if ((s_type == 0xff || s_type == type) &&
            (s_what == 0xff || s_what == what) &&
            (s_on_who == 0xff || s_on_who == on_who))
            return 1;
    }

    return 0;
}



/*
  return objects on spot
*/

Uint8 *
spot_get_objects(CommandSpot * spot, int *to_buy)
{
    if (!spot_objects || *spot_objects == 0xff) {
        if (!buy_objects || *buy_objects == 0xff)
            return NULL;
        *to_buy = 1;
        return buy_objects;
    }
    *to_buy = 0;
    return spot_objects;
}