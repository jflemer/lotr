/****************************************************************************

    lord.h
    some common definitions


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


#ifndef _LORD_H
#define _LORD_H


/* define this to compile The Two Towers - not working */
// #define TTT

#ifndef TTT
/* define this to compile the CD version */
/* works only with Fellowship of the Ring */
// #define CD_VERSION
#endif


/* uncomment for fullscreen */
// #define FULLSCREEN

/* not much tested with other dimensions */
/* larger screen is considered cheating */
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

/* Scaling factor for the screen */
#define SCREEN_FACT 2

/* frame time in milliseconds */
#define FRAME_TIME 50

#if DEMO
#define DATA_DIRECTORY "./"
#else
#ifndef TTT
#ifndef CD_VERSION
#define DATA_DIRECTORY PREFIX "/share/games/lord"
#else
#define DATA_DIRECTORY PREFIX "/share/games/lord/cd"
#endif
#else
#define DATA_DIRECTORY PREFIX "/share/games/lord/towers"
#endif
#endif

/* should we produce pixel-precise interface ass original
   or improve the original a bit */
#define PIXEL_PRECISE 0

/* uncomment to turn on debugging */
/* DEBUG disables SDL parachute
      (so that the game creates coredumps on crash) */
// #define DEBUG

#ifdef DEBUG

/* enable cheats */
#define ENABLE_CHEATS

/* display command spots */
#define DRAW_SPOTS

/* allow wizard mode */
#define WIZARD_MODE

#endif




#ifdef EXTENDED
#include "Python.h"
#include "pythonspot.h"
#endif
#include <SDL_types.h>


#endif /* _LORD_H */
