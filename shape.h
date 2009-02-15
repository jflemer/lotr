/****************************************************************************

    shape.h
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


#ifndef _SHAPES_H
#define _SHAPES_H


#include "graphics.h"


typedef struct
{
    Pixmap *pixmaps[4];
    int pixmaps_num;
} Shape;




typedef struct
{
    Pixmap *pixmap;
    Palette *palette;
    int freepalette;
} Portrait;





/* init shapes */
extern void shapes_init(void);

/* close shapes */
extern void shapes_close(void);

/* set the shapes palette */
extern void shapes_set_palette(void);



/* get shape */
extern Shape *shape_get(int i);

/* get weapon shape corresponding to character shape */
extern int shape_get_weapon_shape_id(int character_shape_id,
                                     int weapon_class);

/* draw shape */
extern void shape_draw(Shape *shape, int state, int x, int y);

/* draw portrait */
extern void portrait_draw(int index, int x, int y);




#endif /* _SHAPES_H */
