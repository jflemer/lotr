/****************************************************************************
 
    combat.h
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


#ifndef _COMBAT_H
#define _COMBAT_H

#include "character.h"

/* start combat */
extern void combat_start(void);

/* genocide monsters */
extern void combat_genocide(void);

/* add new enemy -- returns new id */
extern int combat_enemy(Character * character, int x, int y, int dir,
                        int map_id);

/* return enemy */
extern Character *combat_get_enemy(int id);

/* is combat mode? */
extern int combat_get_mode();

/* combat mode frame */
extern int combat_frame();

/* combat character finished turn */
extern void combat_character_finished();

/* character attack another character */
extern void combat_attack(Character * who, Character * whom);

/* get enemies in bow range */
extern int combat_in_bow_range(Character * character, int codes[10]);

/* proceed all combat frames */
extern void combat_proceed_frames(void);



#endif /* _COMBAT_H */
