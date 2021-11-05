/****************************************************************************

    midi.h
    midi music support


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
#include "midi.h"
#ifdef HAVE_SDL_MIXER
#include <SDL_mixer.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

int midi_disabled = 0;

#ifdef HAVE_SDL_MIXER
static Mix_Music *music = 0;
#endif

static int music_running = 0;

/*
  hook for music finished event
 */
void
hook_music_finished()
{
    music_running = 0;
}

/*
  stops music return nonzero if music was playing
 */
int
stop_midi(void)
{
    int was_running = music_running;

#ifdef HAVE_SDL_MIXER
    if (!music)
        return 0;

    Mix_HaltMusic();
    Mix_FreeMusic(music);
    music = 0;
#endif

    /* HACK: hook_music_finished should be called on Mix_HaltMusic
       according to documentation but it is not */
    hook_music_finished();

    return was_running;
}


/*
  start music
*/
void
play_midi(Uint8 *data, int size, int loop)
{
    if (midi_disabled)
        return;

    if (data == NULL || size == 0) {
#ifdef CD_VERSION
        return;
#endif

#ifndef TTT
        fprintf(stderr, "lotr: trying to play an empty midi file\n");
        exit(1);
#else
        return;
#endif
    }

    stop_midi();

#ifdef HAVE_SDL_MIXER
    Mix_HookMusicFinished(hook_music_finished);

    music = Mix_LoadMUS_RW(SDL_RWFromConstMem(data, size));
    if (!music) {
        fprintf(stderr, "Can't load midi file: %s\n", Mix_GetError());
        return;
    }

    music_running = 1;

    if (Mix_PlayMusic(music, loop ? -1 : 1) < 0) {
        printf("Can't play midifile: %s\n", Mix_GetError());
        hook_music_finished();
    }
#else
    hook_music_finished();
#endif

}
