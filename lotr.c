
/****************************************************************************

    lotr.c
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



/* lotr.h must be included first */
#include "lotr.h"
#include <SDL.h>
#include "cartoon.h"
#include "init.h"
#include "game.h"
#include "gui.h"
#include "lotr_sdl.h"
#include "timing.h"
#include "utils.h"

#ifdef CD_VERSION
#include "av.h"
#endif

#include <stdlib.h>



int
main(int argc, char *argv[])
{
    int game_num = 0;

    /* Check if game data is present */
    if (!lotr_file_exists("arts.dat")) {
        fprintf(stderr, "Cannot find file \"arts.dat\"\n");
        fprintf(stderr, "Please copy game data into the directory \"%s\"\n", DATA_DIRECTORY);
#ifdef WIN32
        system("pause");
#endif
        exit(1);
    }

    lotr_init();

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
        cartoon_play("opening");
#else
    av_play("sacf");
    av_play("saqf");
#endif
#else
    cartoon_play("democrt0");
#endif
#endif


    game_init_graphics();

    lotr_reset_timer();
    while (!lotr_key_esc()) {
        int frame_time = FRAME_TIME;
        lotr_poll_events();
        game_next_frame();

#ifndef ENABLE_CHEATS
        /* Shift key is used for fast movement while cheating */
#if !PIXEL_PRECISE
        if (lotr_key_shift())
            frame_time /= 4;
#endif
#endif

        lotr_timer(frame_time);
    }


    lotr_close();

    return 0;
}
