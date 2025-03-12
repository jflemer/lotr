/****************************************************************************

    init.c
    initializations / deinitializations


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
#include "character.h"
#include "lotr_sdl.h"
#include "shape.h"
#include "sound.h"
#include "init.h"
#include "graphics.h"
#include "gui.h"
#include "map.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>


int initlevel = 0;

/*
  initialize system
*/

void
lotr_init(void)
{
#ifdef EXTENDED
    FILE *test;
#endif
    initlevel = 0;

    /* Clean up on exit */
    atexit(lotr_close);

    lotr_system_init();
    initlevel++;

    sound_init();
    initlevel++;

    graphics_init();
    gui_init();
    initlevel++;

    map_init();
    initlevel++;

    shapes_init();
    initlevel++;

    characters_init();
    initlevel++;

#ifdef EXTENDED
    /* test if spots.py and spot_definitons.py is present */
    test = lotr_fopen("spots.py", "r");
    fclose(test);
    test = lotr_fopen("spot_definitons.py", "r");
    fclose(test);
    Py_Initialize();
    initpythonspot();           /* pyrex module initialization */
    pythonspot_init();
    initlevel++;
#endif


}







/*
  quit system
*/

void
lotr_close(void)
{

#ifdef EXTENDED
    if (initlevel-- >= 7) {
        pythonspot_close();
        Py_Finalize();
    }
#endif

    if (initlevel-- >= 6)
        characters_close();

    if (initlevel-- >= 5)
        shapes_close();

    if (initlevel-- >= 4)
        map_close();

    if (initlevel-- >= 3)
        gui_close();

    if (initlevel-- >= 2)
        sound_close();

    if (initlevel-- >= 1)
        lotr_system_close();

}
