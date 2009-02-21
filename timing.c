/****************************************************************************

    timing.c
    several time functions


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
#include "timing.h"
#include <SDL.h>



static int lord_ticks;

/*
  returns number of second since time 0
 */




/*
  sleep specified number of miliseconds
*/

void
lord_sleep(int delay)
{
    SDL_Delay(delay);
}


/*
  resets timer
*/

void
lord_reset_timer(void)
{
    lord_ticks = SDL_GetTicks();
}


/*
  wait delay milliseconds from the last lord_timer call
*/

void
lord_timer(int delay)
{
    int timer;

    timer = SDL_GetTicks();

    if (lord_ticks + delay > timer) {
        /* we wait some time */
        SDL_Delay(lord_ticks + delay - timer);
        lord_ticks = lord_ticks + delay;
    } else {
        lord_ticks = timer;
    }

}
