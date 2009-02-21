/****************************************************************************

    cartoon_play.c
    cartoon player


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


#include <SDL.h>

#include "lotr.h"
#include "lotr_sdl.h"
#include "cartoon.h"
#include "sound.h"
#include <string.h>
#include <stdlib.h>




int initlevel;


/*
  quit system
*/

void
Cartoon_Close(void)
{

    if (initlevel-- >= 2)
        sound_close();

    if (initlevel-- >= 1)
        lotr_system_close();

}


void
Cartoon_Init(void)
{

    initlevel = 0;

    /* Clean up on exit */
    atexit(Cartoon_Close);

    lotr_system_init();
    initlevel++;

    sound_init();
    initlevel++;

}








int
main(int argc, char *argv[])
{
    int i;

    Cartoon_Init();


    if (argc != 2) {
        printf("Usage: cartoon_play <cartoon>\n");
        return 1;
    }

    for (i = strlen(argv[1]); i > 0; --i)
        if (argv[1][i] == '.') {
            argv[1][i] = 0;
            break;
        }

    play_music();

    cartoon_play(argv[1]);



    return 0;
}
