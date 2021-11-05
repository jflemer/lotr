/****************************************************************************

    lotr_sdl.h
    system dependent part via SDL


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


#ifndef _LORD_SDL_H
#define _LORD_SDL_H

#include <SDL.h>

#if SDL_MAJOR_VERSION == 2
#  include <SDL_keyboard.h>
#else
#  include <SDL_keysym.h>
#endif


#define KEY_ENTER       SDLK_RETURN
#define KEY_BACKSPACE   SDLK_BACKSPACE

#define KEY_LEFT        SDLK_LEFT
#define KEY_RIGHT       SDLK_RIGHT
#define KEY_UP          SDLK_UP
#define KEY_DOWN        SDLK_DOWN
#define KEY_PAGEUP      SDLK_PAGEUP
#define KEY_PAGEDOWN    SDLK_PAGEDOWN


/* initialize SDL */
extern void lotr_system_init(void);

/* quit SDL */
extern void lotr_system_close(void);


/* play sound sample */
extern void play_sample(Uint8 *data, int size);

/* stop sample */
extern void stop_sample(Uint8 *data);

/* are we playing a sound? */
extern int playing_sample(void);


/* polls all pending events */
extern void lotr_poll_events(void);

/* resets keyboard status */
extern void lotr_reset_keyboard(void);

/* was a key pressed? */
extern int lotr_kb_hit(void);

/* get last pressed key - return 0 if none */
extern int lotr_get_key(void);

/* disable keyboard input */
extern void lotr_input_disable(void);

/* enable keyboard input */
extern void lotr_input_enable(void);



/* return key states */
extern int lotr_key_left();
extern int lotr_key_right();
extern int lotr_key_up();
extern int lotr_key_down();
extern int lotr_key_esc();
extern int lotr_key_shift();
extern int lotr_key_ctrl();




/* shows a new screen */
extern void lotr_show_screen(Uint8 *newscreen);


/* sets palette */
extern void lotr_system_set_palette(Uint8 *palette, int firstcolor,
                                    int ncolors);




#endif /* _LORD_SDL_H */
