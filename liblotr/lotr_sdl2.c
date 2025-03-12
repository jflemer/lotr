/****************************************************************************

    lotr_sdl.c
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


#include "lotr.h"
#include "lotr_sdl.h"
#include "midi.h"
#ifdef HAVE_LIBHQX
#include "hqx.h"
#endif
#include <SDL.h>
#ifdef HAVE_LIBSDL_MIXER
#include <SDL_mixer.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


/* main window */
static SDL_Window *main_window = NULL;
static SDL_Renderer *main_window_renderer = NULL;

/* palette */
SDL_Color active_palette[256];

/* keyboard buffer */
#define KEYBOARDBUFFERESIZE 16
static int lotr_keybufferpos = 0;
static int lotr_keybuffer[KEYBOARDBUFFERESIZE];


#ifdef HAVE_LIBSDL_MIXER
Mix_Chunk *sound_samples[MIX_CHANNELS];
#endif


/* indicates which keys were pressed */

static int lotr_input_disabled = 0;

static int lotr_key_left_pressed;
static int lotr_key_right_pressed;
static int lotr_key_up_pressed;
static int lotr_key_down_pressed;

static int lotr_key_esc_pressed;
static int lotr_key_shift_pressed;
static int lotr_key_ctrl_pressed;


/*
  audio
*/

#ifdef HAVE_LIBSDL_MIXER
void
hook_channel_finished(int channel)
{
    if (channel < 0 || channel > MIX_CHANNELS || !sound_samples[channel]) {
        fprintf(stderr,
                "Non-existend or non-playing sound channel finished?\n");
        return;
    }

    Mix_FreeChunk(sound_samples[channel]);
    sound_samples[channel] = 0;
}
#endif

/*
  initialize SDL
*/
void
lotr_system_init(void)
{
    int i;

    SDL_AudioSpec desired;

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

    unsigned win_flags = 0;
    int win_fact = SCREEN_FACT;

#ifdef HAVE_LIBHQX
    hqxInit();
#endif
#ifdef FULLSCREEN
    win_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif
    win_flags |= SDL_WINDOW_RESIZABLE;

    main_window =
        SDL_CreateWindow("Lord of the Rings",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH * win_fact, SCREEN_HEIGHT * win_fact,
                win_flags);

    if (main_window == NULL) {
        fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
        exit(1);
    }

    main_window_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
    if (main_window_renderer == NULL) {
        fprintf(stderr, "Couldn't get window renderer: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_RenderSetLogicalSize(main_window_renderer,
            SCREEN_WIDTH * win_fact, SCREEN_HEIGHT * win_fact);

    memset(active_palette, '\0', sizeof(active_palette));

    lotr_reset_keyboard();

    desired.freq = 22050;
#ifdef CD_VERSION
    desired.format = AUDIO_S8;
#else
    desired.format = AUDIO_U8;
#endif
    desired.samples = 4096;
    desired.userdata = NULL;
    desired.channels = 1;

#ifdef HAVE_LIBSDL_MIXER
    if (Mix_OpenAudio(desired.freq, desired.format, 1, desired.samples) < 0) {
        fprintf(stderr, "lotr: could not initialize mixer: %s\n",
                SDL_GetError());
        fprintf(stderr, "lotr: sfx and music disabled\n");
        midi_disabled = 1;
    }

    for (i = 0; i < MIX_CHANNELS; ++i)
        sound_samples[i] = 0;

    Mix_ChannelFinished(hook_channel_finished);
#endif

    lotr_input_disabled = 0;

    srand(time(NULL));
}



/*
  quit SDL
*/
void
lotr_system_close(void)
{
#ifdef HAVE_LIBSDL_MIXER
    if (!midi_disabled)
        Mix_CloseAudio();
#endif

    /* Shutdown all subsystems */
    SDL_DestroyWindow(main_window);
    SDL_Quit();
}



/*
  play sound sample
*/
void
play_sample(Uint8 *data, int size)
{
#ifdef HAVE_LIBSDL_MIXER
    Mix_Chunk *sample;
    int channel;

    /* Do not play non-existent or too short samples */
    if (data == NULL || size <= 0)
        return;

    if ((sample = Mix_QuickLoad_RAW(data, (Uint32) size)) == NULL) {
        fprintf(stderr, "Can not load sound sample: %s\n", Mix_GetError());
        return;
    }

    if ((channel = Mix_PlayChannel(-1, sample, 0)) < 0) {
        fprintf(stderr, "Can not play sound sample: %s\n", Mix_GetError());
        Mix_FreeChunk(sample);
        return;
    }

    if (channel >= MIX_CHANNELS) {
        fprintf(stderr, "Mix_PlayChannel returned too big channel number\n");
        return;
    }

    sound_samples[channel] = sample;
#endif
}


/*
   stop sound sample
 */
void
stop_sample(Uint8 *data)
{
#ifdef HAVE_LIBSDL_MIXER
    int i;
    for (i = 0; i < MIX_CHANNELS; ++i)
        if (sound_samples[i] && sound_samples[i]->abuf == data)
            Mix_HaltChannel(i);
#endif
}

/*
   are we playing a sound?
*/
int
playing_sample(void)
{
    int res = 0;

#ifdef HAVE_LIBSDL_MIXER
    int i;
    for (i = 0; i < MIX_CHANNELS; ++i) {
        if (sound_samples[i]) {
            res = 1;
            break;
        }
    }
#endif

    return res;
}



/*
  polls all pending events
*/
void
lotr_poll_events(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:

                if (lotr_input_disabled)
                    break;

                if (lotr_keybufferpos < KEYBOARDBUFFERESIZE) {
                    int keysym = event.key.keysym.sym;

                    /* Map numeric keyboard to normal numbers */
                    if (keysym >= SDLK_KP_0 && keysym <= SDLK_KP_9)
                        keysym = keysym - SDLK_KP_0 + '0';

                    lotr_keybuffer[lotr_keybufferpos++] = keysym;
                }

                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        lotr_key_esc_pressed = 1;
                        break;
                    case SDLK_LEFT:
                        lotr_key_left_pressed = 1;
                        break;
                    case SDLK_RIGHT:
                        lotr_key_right_pressed = 1;
                        break;
                    case SDLK_UP:
                        lotr_key_up_pressed = 1;
                        break;
                    case SDLK_DOWN:
                        lotr_key_down_pressed = 1;
                        break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        lotr_key_shift_pressed = 1;
                        break;
                    case SDLK_LCTRL:
                    case SDLK_RCTRL:
                        lotr_key_ctrl_pressed = 1;
                        break;

                    default:
                        break;
                }

                break;


            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        lotr_key_left_pressed = 0;
                        break;
                    case SDLK_RIGHT:
                        lotr_key_right_pressed = 0;
                        break;
                    case SDLK_UP:
                        lotr_key_up_pressed = 0;
                        break;
                    case SDLK_DOWN:
                        lotr_key_down_pressed = 0;
                        break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        lotr_key_shift_pressed = 0;
                        break;
                    case SDLK_LCTRL:
                    case SDLK_RCTRL:
                        lotr_key_ctrl_pressed = 0;
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
lotr_reset_keyboard(void)
{
    lotr_poll_events();

    lotr_keybufferpos = 0;

    lotr_key_left_pressed = 0;
    lotr_key_right_pressed = 0;
    lotr_key_up_pressed = 0;
    lotr_key_down_pressed = 0;

    lotr_key_esc_pressed = 0;

}


/*
  was a key pressed?
*/
int
lotr_kb_hit(void)
{
    lotr_poll_events();

    return lotr_keybufferpos != 0;
}


/*
  get last pressed key - return 0 if none
*/
int
lotr_get_key(void)
{
    int i;
    int key;

    lotr_poll_events();

    if (lotr_keybufferpos == 0)
        return 0;

    key = lotr_keybuffer[0];
    lotr_keybufferpos--;

    for (i = 0; i < lotr_keybufferpos; ++i)
        lotr_keybuffer[i] = lotr_keybuffer[i + 1];

    return key;

}


/*
  disable keyboard input
*/
void
lotr_input_disable(void)
{
    lotr_input_disabled = 1;
}

/*
  enable keyboard input
*/
void
lotr_input_enable(void)
{
    lotr_input_disabled = 0;
}


/*
  shows a new screen
*/
void
lotr_show_screen(Uint8 *newscreen)
{
    SDL_Surface *src;
#if SCREEN_FACT == 1
    /* convert pixels to 8-bit SDL surface */
    src = SDL_CreateRGBSurfaceFrom(
        newscreen, SCREEN_WIDTH, SCREEN_HEIGHT, 8, SCREEN_WIDTH, 0, 0, 0, 0);
    SDL_SetPaletteColors(src->format->palette, active_palette, 0, 256);
#elif defined(HAVE_LIBHQX)
    /* convert pixels to 8-bit SDL surface */
    SDL_Surface *src8 = SDL_CreateRGBSurfaceFrom(
        newscreen, SCREEN_WIDTH, SCREEN_HEIGHT, 8, SCREEN_WIDTH, 0, 0, 0, 0);
    SDL_SetPaletteColors(src8->format->palette, active_palette, 0, 256);
    /* output surface */
    src = SDL_CreateRGBSurface(
        0, SCREEN_WIDTH * SCREEN_FACT, SCREEN_HEIGHT * SCREEN_FACT, 32, 0, 0, 0, 0);
    /* convert 8-bit SDL surface to 32-bit SDL */
    SDL_Surface *src32 = SDL_ConvertSurface(src8, src->format, SDL_SWSURFACE);
    /* apply HQX scalling from 32-bit source to screen */
# if SCREEN_FACT == 2
    hq2x_32(src32->pixels, src->pixels, SCREEN_WIDTH, SCREEN_HEIGHT); 
# elif SCREEN_FACT == 3
    hq3x_32(src32->pixels, src->pixels, SCREEN_WIDTH, SCREEN_HEIGHT); 
# elif SCREEN_FACT == 4
    hq4x_32(src32->pixels, src->pixels, SCREEN_WIDTH, SCREEN_HEIGHT); 
# else
#  error SCREEN_FACT must be 2, 3 or 4 for HQX
# endif
    SDL_FreeSurface(src8);
    SDL_FreeSurface(src32);
#else
    /* convert pixels to 8-bit SDL surface */
    src = SDL_CreateRGBSurface(
        0, SCREEN_WIDTH * SCREEN_FACT, SCREEN_HEIGHT * SCREEN_FACT, 8, 0, 0, 0, 0);
    SDL_SetPaletteColors(src->format->palette, active_palette, 0, 256);

    int i, j;
    Uint8 tmpscreen[SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_FACT];
    Uint8 *ts;
    Uint8 *pixels;

    pixels = src->pixels;


    /* multiply width */
    ts = tmpscreen;
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
        for (j = 0; j < SCREEN_FACT; ++j, ++ts)
            *ts = newscreen[i];


    /* multiply height */
    ts = tmpscreen;
    for (i = 0; i < SCREEN_HEIGHT; ++i) {
        for (j = 0; j < SCREEN_FACT; ++j) {
            memcpy(pixels, ts, SCREEN_WIDTH * SCREEN_FACT);
            pixels += src->pitch;
        }
        ts += SCREEN_WIDTH * SCREEN_FACT;
    }
#endif

    SDL_Texture *texture = SDL_CreateTextureFromSurface(main_window_renderer, src);
    SDL_RenderClear(main_window_renderer);
    SDL_RenderCopy(main_window_renderer, texture, NULL, NULL);
    SDL_RenderPresent(main_window_renderer);

    /* cleanup */
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(src);
}


/*
  return key states
*/

int
lotr_key_left()
{
    return lotr_key_left_pressed;
}

int
lotr_key_right()
{
    return lotr_key_right_pressed;
}

int
lotr_key_up()
{
    return lotr_key_up_pressed;
}

int
lotr_key_down()
{
    return lotr_key_down_pressed;
}

int
lotr_key_esc()
{
    return lotr_key_esc_pressed;
}

int
lotr_key_shift()
{
    return lotr_key_shift_pressed;
}

int
lotr_key_ctrl()
{
    return lotr_key_ctrl_pressed;
}


/*
  sets palette
*/
void
lotr_system_set_palette(Uint8 *palette, int firstcolor, int ncolors)
{
    int i, c;

    if (ncolors > 0x100)
        return;

    for (i = 0; i < ncolors; ++i) {
        c = firstcolor + i;
        active_palette[c].r = palette[3 * (i + firstcolor) + 0] * 4;
        active_palette[c].g = palette[3 * (i + firstcolor) + 1] * 4;
        active_palette[c].b = palette[3 * (i + firstcolor) + 2] * 4;
    }
}
