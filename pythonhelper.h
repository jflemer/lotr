/****************************************************************************

    pythonhelper.h
    passes some definitions from lotr.h to python


    Lord of the Rings game engine

    Copyright (C) 2004  Michal Benes

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


#ifndef _PYTHONHELPER_H
#define _PYTHONHELPER_H

#include "spot.h"


extern char *ph_data_directory(void);

extern void ph_init_spot(CommandSpot spot, int map, int x, int y, int w,
                         int h);

#endif /* _PYTHONHELPER_H */
