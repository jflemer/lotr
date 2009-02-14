/****************************************************************************

    lord_sdl.c
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


#include "lord.h"
#include "lord_sdl.h"
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


/* main window */
SDL_Surface *main_display;


/* keyboard buffer */
#define KEYBOARDBUFFERESIZE 16
int lord_keybufferpos = 0;
int lord_keybuffer[KEYBOARDBUFFERESIZE];




/* indicates which keys were pressed */

int lord_input_disabled = 0;

int lord_key_left;
int lord_key_right;
int lord_key_up;
int lord_key_down;

int lord_key_esc;
int lord_key_shift;
int lord_key_ctrl;


/*
  audio
*/

#define AUDIO_BUF 1024
Uint8 *lord_audio_data;
volatile int lord_audio_size;
volatile int lord_audio_pos;


/*
  update sound
 */

void
SDL_Sound_Update(void *userdata, Uint8 * stream, int len)
{

    int l;

    l = lord_audio_size - lord_audio_pos;

    if (len > l)
        len = l;

    if (len == 0)
        SDL_PauseAudio(1);

    SDL_MixAudio(stream, lord_audio_data + lord_audio_pos, len,
                 SDL_MIX_MAXVOLUME);
    lord_audio_pos += len;
}




/*
  initialize SDL
*/

void
System_Init(void)
{

    SDL_AudioSpec desired, obtained;

#ifdef DEBUG
    if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE) ==
         -1))
#else
    if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1))
#endif
    {
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }

    /* Initialize the display in a 8-bit palettized mode */

#ifdef FULLSCREEN
    main_display =
        SDL_SetVideoMode(SCREEN_WIDTH * SCREEN_FACT,
                         SCREEN_HEIGHT * SCREEN_FACT, 8,
                         SDL_SWSURFACE | SDL_FULLSCREEN);
#else
    main_display =
        SDL_SetVideoMode(SCREEN_WIDTH * SCREEN_FACT,
                         SCREEN_HEIGHT * SCREEN_FACT, 8, SDL_SWSURFACE);
#endif

    if (main_display == NULL) {
        fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_WM_SetCaption("Lord of the Rings", "LotR");
    SDL_WM_SetIcon(SDL_LoadBMP("ring.bmp"), NULL);


    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
                        SDL_DEFAULT_REPEAT_INTERVAL);
    ResetKeyboard();

#ifndef CD_VERSION
    desired.freq = 22050;
    desired.format = AUDIO_S8;
#else
    desired.freq = 11025;
    desired.format = AUDIO_U8;
#endif
    desired.samples = AUDIO_BUF;
    desired.callback = SDL_Sound_Update;
    desired.userdata = NULL;
    desired.channels = 1;

    if (SDL_OpenAudio(&desired, &obtained) < 0) {
        fprintf(stderr, "lord: could not open audio, sfx disabled\n");
    }

    lord_input_disabled = 0;

    srand(time(NULL));

}






/*
  quit SDL
*/

void
System_Close(void)
{
    /* Shutdown all subsystems */
    SDL_Quit();
}



/*
  play sound sample
*/

void
PlaySample(Uint8 * data, int size)
{

    if (data == NULL)
        return;

    SDL_PauseAudio(0);
    lord_audio_data = data;
    lord_audio_size = size;
    lord_audio_pos = 0;
}



/*
  resize sound sample
*/

void
ResizeSample(Uint8 * data, int size)
{
    if (data == NULL) {
        SDL_PauseAudio(1);
        lord_audio_pos = lord_audio_size;
        return;
    }

    lord_audio_data = data;

    if (size < lord_audio_pos) {
        SDL_PauseAudio(1);
        lord_audio_size = lord_audio_pos = size;
        return;
    }

    if (lord_audio_pos == lord_audio_size && size > lord_audio_size)
        SDL_PauseAudio(0);

    lord_audio_size = size;

}


/*
   are we playing a sound?
*/

int
PlayingSample(void)
{
    return lord_audio_pos < lord_audio_size;
}



/*
  polls all pending events
*/

void
PollEvents(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:

            if (lord_input_disabled)
                break;

            if (lord_keybufferpos < KEYBOARDBUFFERESIZE) {
                lord_keybuffer[lord_keybufferpos++] = event.key.keysym.sym;
            }

            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                lord_key_esc = 1;
                break;
            case SDLK_LEFT:
                lord_key_left = 1;
                break;
            case SDLK_RIGHT:
                lord_key_right = 1;
                break;
            case SDLK_UP:
                lord_key_up = 1;
                break;
            case SDLK_DOWN:
                lord_key_down = 1;
                break;
            case SDLK_LSHIFT:
            case SDLK_RSHIFT:
                lord_key_shift = 1;
                break;
            case SDLK_LCTRL:
            case SDLK_RCTRL:
                lord_key_ctrl = 1;
                break;

            default:
                break;
            }

            break;


        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                lord_key_left = 0;
                break;
            case SDLK_RIGHT:
                lord_key_right = 0;
                break;
            case SDLK_UP:
                lord_key_up = 0;
                break;
            case SDLK_DOWN:
                lord_key_down = 0;
                break;
            case SDLK_LSHIFT:
            case SDLK_RSHIFT:
                lord_key_shift = 0;
                break;
            case SDLK_LCTRL:
            case SDLK_RCTRL:
                lord_key_ctrl = 0;
                break;
            case SDLK_ESCAPE:
                /* esc can be only reseted */
                break;
            default:
                break;
            }

            break;

            /* SDL_QUIT event (window close) */
        case SDL_QUIT:
            exit(0);
            break;

        default:
            break;
        }

    }

}





/*
  resets keyboard status
*/

void
ResetKeyboard(void)
{
    PollEvents();

    lord_keybufferpos = 0;

    lord_key_left = 0;
    lord_key_right = 0;
    lord_key_up = 0;
    lord_key_down = 0;

    lord_key_esc = 0;

}

/*
  was a key pressed?
*/

int
KbHit(void)
{
    PollEvents();

    return lord_keybufferpos != 0;
}


/*
  get last pressed key - return 0 if none
*/

int
GetKey(void)
{
    int i;
    int key;

    PollEvents();

    if (lord_keybufferpos == 0)
        return 0;

    key = lord_keybuffer[0];
    lord_keybufferpos--;

    for (i = 0; i < lord_keybufferpos; ++i)
        lord_keybuffer[i] = lord_keybuffer[i + 1];

    return key;

}



/*
  disable keyboard input
*/

void
InputDisable(void)
{
    lord_input_disabled = 1;
}

/*
  enable keyboard input
*/

void
InputEnable(void)
{
    lord_input_disabled = 0;
}





/*
  shows a new screen
*/

void
ShowScreen(Uint8 * newscreen)
{
    int i, j;
    Uint8 tmpscreen[SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_FACT];
    Uint8 *ts;
    Uint8 *pixels;

    pixels = main_display->pixels;


    ts = tmpscreen;
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
        for (j = 0; j < SCREEN_FACT; ++j, ++ts)
            *ts = newscreen[i];


    ts = tmpscreen;
    for (i = 0; i < SCREEN_HEIGHT; ++i) {
        for (j = 0; j < SCREEN_FACT; ++j) {
            memcpy(pixels, ts, SCREEN_WIDTH * SCREEN_FACT);
            pixels += main_display->pitch;
        }
        ts += SCREEN_WIDTH * SCREEN_FACT;
    }

    SDL_UpdateRect(main_display, 0, 0, SCREEN_WIDTH * SCREEN_FACT,
                   SCREEN_HEIGHT * SCREEN_FACT);

}




/*
  return key states
*/

int
KeyLeft()
{
    return lord_key_left;
}

int
KeyRight()
{
    return lord_key_right;
}

int
KeyUp()
{
    return lord_key_up;
}

int
KeyDown()
{
    return lord_key_down;
}

int
KeyEsc()
{
    return lord_key_esc;
}

int
KeyShift()
{
    return lord_key_shift;
}

int
KeyCtrl()
{
    return lord_key_ctrl;
}





/*
  sets palette
*/

void
SystemSetPalette(Uint8 * palette, int firstcolor, int ncolors)
{
    int i;
    SDL_Color colors[256];

    if (ncolors > 0x100)
        return;

    for (i = 0; i < ncolors; ++i) {
        colors[i].r = palette[3 * (i + firstcolor) + 0] * 4;
        colors[i].g = palette[3 * (i + firstcolor) + 1] * 4;
        colors[i].b = palette[3 * (i + firstcolor) + 2] * 4;
    }

    SDL_SetPalette(main_display, SDL_PHYSPAL, colors, firstcolor, ncolors);

}
