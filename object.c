/****************************************************************************
 
    object.c
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

#include "lord.h"
#include <string.h>


#include "object_names.h"

#ifndef TTT
#include "object_prices.h"
char **object_names = object_names1;
#else
#include "object_names_ttt.h"
char **object_names = object_names2;
#endif


/*
  get object name
*/

char *
object_name(int index)
{
    if (index < 0 || index > 0xf6)
        return "";
    return object_names[index];
}


/*
  get object price
*/

int
object_price(int index)
{

#ifndef TTT

    if (index < 0x60 || index >= 0x100)
        return 255;

    return object_prices[index - 0x60];

#else

    return 255;

#endif
}

/*
  is the object The One Ring
*/

int
object_is_ring(int index)
{
    return index == 0x94;
}


/*
  the object is armour
*/

int
object_is_armour(int index)
{
    return index >= 0x80 && index < 0x88;
}

/*
  the object is shield
*/

int
object_is_shield(int index)
{
    return index == 0x88 || index == 0x89;
}


/*
  the object is weapon
*/

int
object_is_weapon(int index)
{
    return index >= 0x60 && index < 0x80;
}


/*
  get weapon class
*/

int
object_weapon_class(int index)
{

    /* Sting, Barrow Dagger */
    if (index == 0x60 || index == 0x61 || index == 79)
        return 1;

    /* Anduril, Glamdring, Spider sword, Troll slayer, Magic sword, Herugrim */
    if (index == 0x62 || index == 0x63 || index == 0x64 || index == 0x66
        || index == 0x69 || index == 0x6a)
        return 2;

    /* Durin's axe  */
    if (index == 0x67)
        return 4;

    /* Magic Bow  */
    if (index == 0x65)
        return 8;


    if (index >= 0x70 && index <= 0x78)
        return index - 0x70 + 1;

    return 0;
}


/*
  a skill associated with the weapon
*/

int
object_weapon_skill(int index)
{
    int class;
    class = object_weapon_class(index);

    switch (class) {

    case 2:                    /* sword */
        return 0x41;

#ifdef TTT
    case 3:                    /* staff */
        return 0x44;
#endif

    case 4:                    /* axe */
        return 0x42;

    case 5:                    /* mace */
        return 0x43;

#ifndef TTT
    case 7:                    /* flail */
        return 0x44;
#endif

    case 8:                    /* bow */
        return 0x45;

    default:
        return 0;

    }


    return 0;
}



/*
  the object is skill
*/

int
object_is_skill(int index)
{
    return index >= 0x40 && index < 0x60;
}


/*
  the object is spell
*/

int
object_is_spell(int index)
{
    return index >= 1 && index < 0x20;
}



/*
  the object is item
*/

int
object_is_item(int index)
{
    return index >= 0x60;
}



/*
  food value
*/

int
object_food_value(int index)
{
    if (index == 0xac)
        return 1;
    if (index == 0xa4 || index == 0xec || index == 0xb8)
        return 2;
    if (index == 0xe1 || index == 0xb5)
        return 3;
    if (index == 0xb7 || index == 0xbe)
        return 4;
    if (index == 0xf5)
        return 6;
    return 0;
}



/*
  weight of a weapon
*/

int
object_weapon_weight(int index)
{
    int class, w;
    class = object_weapon_class(index);

    if (class == 0)
        return 0;

    switch (class) {
    case 1:                    /* dagger */
        w = 0;
        break;

    case 2:                    /* sword */
        w = 16;
        break;

    case 3:                    /* staff */
        w = 7;
        break;

    case 4:                    /* axe */
        w = 20;
        break;

    case 5:                    /* mace */
        w = 13;
        break;

    case 6:                    /* club */
        w = 8;
        break;

    case 7:                    /* flail */
        w = 16;
        break;

    case 8:                    /* bow */
        w = 7;
        break;

    case 9:                    /* torch */
        w = 0;
        break;

    default:
        w = 0;
        break;

    }


    /* magic weapons are lighter */

    if (index < 0x70)
        w = w * 7 / 10;

    return w;

}




/*
  damage a weapon causes
*/

int
object_weapon_damage(int index)
{
    int class, result;
    class = object_weapon_class(index);

    if (class == 0)
        return 0;

    switch (class) {
    case 1:                    /* dagger */
        result = 6;
        break;

    case 2:                    /* sword */
        result = 8;
        break;

    case 3:                    /* staff */
        result = 6;
        break;

    case 4:                    /* axe */
        result = 10;
        break;

    case 5:                    /* mace */
        result = 8;
        break;

    case 6:                    /* club */
        result = 6;
        break;

    case 7:                    /* flail */
        result = 10;
        break;

    case 8:                    /* bow */
        result = 8;
        break;

    case 9:                    /* torch */
        result = 6;
        break;

    default:
        result = 5;
        break;

    }

    return result;


}




/*
  a weapon to hit bonus
*/

int
object_weapon_to_hit(int index)
{
    int class, result;
    class = object_weapon_class(index);

    if (class == 0)
        return 0;

    result = 0;

    if (class > 0 && class < 4)
        result = 2;


    /* magic weapons have better bonus */
    if (index < 0x70)
        result += 2;


    return result;


}




/* 
   mail damage reduced
*/

int
object_armour_reduced(int index)
{
    if (index <= 0x80 || index > 0x84)
        return 0;

    return index - 0x80 + 1;

}



/* 
   mail effect on to hit bomus
*/

int
object_armour_to_hit(int index)
{
    /* impairing effect of armour */
    if (index == 0x81 || index == 0x83)
        return 1;
    if (index == 0x82)
        return 2;

    return 0;

}



/* 
   shield effect on to hit bonus
*/

int
object_shield_to_hit(int index)
{
    if (index == 0x88)
        return 2;
    if (index == 0x89)
        return 4;

    return 0;

}


#ifdef TTT
/*
  return if object can be transfered from vol. I to vol. II
*/

int
object_transferable(int index)
{
    if (index < 0x60 || index > 0xf6)
        return 0;
    return !strcmp(object_names1[index], object_names2[index]);
}
#endif
