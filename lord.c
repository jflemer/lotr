
/****************************************************************************

    lord.c
    the main module


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



/* lord.h must be included first */
#include "lord.h"
#include <SDL.h>
#include "cartoon.h"
#include "init.h"
#include "game.h"
#include "gui.h"
#include "lord_sdl.h"
#include "timing.h"

#ifdef CD_VERSION
#include "av.h"
#endif

#include <stdlib.h>



int
main(int argc, char *argv[])
{
    int game_num = 0;

    Lord_Init();

#ifdef TTT
    game_num = gui_ttt_start_dialog();
#endif

    game_new();
    if (game_num)
        game_load(game_num);


#ifndef DEBUG
    cartoon_show_credits(1);
#if !DEMO
#ifndef CD_VERSION
    if (game_num == 0)
        playcartoon("opening");
#else
    playav("sacf");
    playav("saqf");
#endif
#else
    playcartoon("democrt0");
#endif
#endif


    game_init_graphics();

    ResetTimer();
    while (!lord_key_esc()) {
        lord_poll_events();
        game_next_frame();

        Timer(FRAME_TIME);
    }


    Lord_Close();

    return 0;
}
