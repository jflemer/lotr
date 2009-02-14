/****************************************************************************
 
    object.h
    handling game objects (skills, spells, items)


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


#ifndef _OBJECT_H
#define _OBJECT_H

#define OBJECT_SILVER      0x00

#define OBJECT_TORCH       0x78

#define SPELL_ILLUMINATE   0x01

/* get object name */
extern char *object_name(int index);

/* get object price */
extern int object_price(int index);

/* the object is The One Ring */
extern int object_is_ring(int index);

/* the object is armour */
extern int object_is_armour(int index);

/* the object is shield */
extern int object_is_shield(int index);

/* the object is weapon */
extern int object_is_weapon(int index);

/* get weapon class */
extern int object_weapon_class(int index);

/* the object is skill*/
extern int object_is_skill(int index);

/* the object is spell*/
extern int object_is_spell(int index);

/* the object is item*/
extern int object_is_item(int index);

/* food value */
extern int object_food_value(int index);

/* weight of a weapon */
extern int object_weapon_weight(int index);

/* damage a weapon causes */
extern int object_weapon_damage(int index);

/* a weapon to hit bonus */
extern int object_weapon_to_hit(int index);

/* a skill associated with the weapon */
extern int object_weapon_skill(int index);

/* mail damage reduced */
extern int object_armour_reduced(int index);

/* mail effect on to hit bonus */
extern int object_armour_to_hit(int index);

/* shield effect on to hit bonus */
extern int object_shield_to_hit(int index);

#ifdef TTT
/* return if object can be transfered from vol. I to vol. II */
extern int object_transferable(int index);
#endif



#endif /* _OBJECT_H */
