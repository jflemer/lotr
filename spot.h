/****************************************************************************

    spot.h
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


#ifndef _SPOT_H
#define _SPOT_H


#include <SDL_types.h>


#define MAX_COMMAND_SPOTS 256
#define MAX_PYTHON_SPOTS 1024
#ifndef TTT
#define SPOT_MAX_COMMANDS 128
#else
#define SPOT_MAX_COMMANDS 256
#endif


/* game command spot */
typedef struct
{
    int id;
    int pythonspot;
    int map;
    int building;
    int floor;
    int flag;
    int not_parsed;
    int pos;
    int headersize;
    Uint8 *data;
    int data_size;
    int commands_num;
    int label_start;
    int if_result;
    int command_start[SPOT_MAX_COMMANDS];
    int command_level[SPOT_MAX_COMMANDS];
} CommandSpot;


#define SPOT_ACTION_OBJECT 0
#define SPOT_ACTION_SKILL  1
#define SPOT_ACTION_MAGIC  3
#define SPOT_ACTION_MOVE   5
#define SPOT_ACTION_TIMER  6
#define SPOT_ACTION_TRADE  7
#define SPOT_ACTION_ATTACK 8
#define SPOT_ACTION_GET    9
#define SPOT_ACTION_DISCARD  0xa
#define SPOT_ACTION_QUESTION 0xb
#define SPOT_ACTION_BUY    0xc
#define SPOT_ACTION_TALK   0xd



/* parse command spots */
extern void spot_parse(Uint8 *data, int size,
                       CommandSpot *spots[], int *spots_num);


/* free command spot */
extern void spot_free(CommandSpot *spot);


/* print command spot to a buffer */
extern char *spot_get_string(CommandSpot *spot);

/* print command spot */
extern void spot_print(CommandSpot *spot);

/* start running command spot */
extern int spot_start(CommandSpot *spot);

/* continue running command spot */
extern int spot_continue(CommandSpot *spot);

/* spot action */
extern int spot_action(CommandSpot *spot, int type, int what, int on_who);

/* can do an action on this spot */
extern int spot_has_action(CommandSpot *spot, int type, int what, int on_who);

/* return objects on spot */
extern Uint8 *spot_get_objects(CommandSpot *spot, int *to_buy);


/* set if result */
extern void spot_if_result(CommandSpot *spot, int result);

/* valid letter in a question */
extern int spot_question_letter(Uint8 c);

/* return character
  (including special characters like leader/ ringbearer, etc...) */
extern Character *spot_character_get(int id);


#endif /* _SPOT_H */
