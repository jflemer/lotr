/****************************************************************************
 
    sound.c
    sound support


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
#include "archive.h"
#include "lord_sdl.h"
#include "midi.h"
#include "sound.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


#ifdef CD_VERSION
#include "sounds_cd.h"
#else
#define SOUNDS_NUM 25
#endif

Uint8 *music_data;
int music_size;

Uint8 *sound_data[SOUNDS_NUM];
int sound_sizes[SOUNDS_NUM];

#define MUSIC_MAIN_INDEX 8


/*
  sound initializations
*/

void
sound_init(void)
{
    int i;

#ifndef TTT

#ifndef CD_VERSION

    Archive *archive;

    archive = ndxarchiveopen("snd");

    for (i = 0; i < SOUNDS_NUM; ++i) {
        sound_data[i] = readarchive(archive, i);
        sound_sizes[i] = archivedatasize(archive, i);
    }

    music_data = sound_data[MUSIC_MAIN_INDEX];
    music_size = sound_sizes[MUSIC_MAIN_INDEX];

    archiveclose(archive);

#else /* ifdef CD_VERSION */

    FILE *adlfile;

    for (i = 0; i < SOUNDS_NUM; ++i) {
        adlfile = lordfopen(sound_names[i], "r");
        sound_sizes[i] = filelen(adlfile);
        sound_data[i] = lordmalloc(sound_sizes[i]);
        if (fread(sound_data[i], 1, sound_sizes[i], adlfile) !=
            sound_sizes[i]) {
            fprintf(stderr, "lord: can not read file %s\n", sound_names[i]);
            perror("lord");
            exit(1);
        }
        fclose(adlfile);
    }

    music_data = NULL;
    music_size = 0;

#endif

#else /* ifdef TTT */

    for (i = 0; i < SOUNDS_NUM; ++i) {
        sound_data[i] = NULL;
        sound_sizes[i] = 0;
    }

    music_data = NULL;
    music_size = 0;

#endif

}


/*
  sound deinitializations
*/

void
sound_close(void)
{

    int i;

    StopMidi();
    for (i = 0; i < SOUNDS_NUM; ++i)
        if (sound_data[i])
            free(sound_data[i]);
}


/*
  sound play
*/

void
sound_play(int index)
{

#ifndef CD_VERSION
    if (index > 8)
        PlaySample(sound_data[index - 8], sound_sizes[index - 8]);
    else
        PlayMidi(sound_data[index + 8], sound_sizes[index + 8]);
#else
    if (index > 11)
        PlaySample(sound_data[index - 11], sound_sizes[index - 11]);
#endif

}



/*
  play the main music track
*/

void
play_music(void)
{
    PlayMidi(music_data, music_size);
}


/*
  starts stop playing the music track
*/

void
toggle_music(void)
{
    if (StopMidi())
        play_music();
}


/*
  play music if spell was performed
*/

void
music_spell_performed(void)
{
#ifndef CD_VERSION
    PlayMidi(sound_data[15], sound_sizes[15]);
#else
    PlaySample(sound_data[0], sound_sizes[0]);
#endif
}


/*
  play music if spell failed
*/

void
music_spell_failed(void)
{
#ifndef CD_VERSION
    PlayMidi(sound_data[16], sound_sizes[16]);
#else
    PlaySample(sound_data[1], sound_sizes[1]);
#endif
}



/*
  play music if combat started
*/

void
music_combat_started(void)
{
#ifndef CD_VERSION
    PlayMidi(sound_data[21], sound_sizes[21]);
#endif
}


/*
  play music if combat was won
 */

void
music_combat_won(void)
{
#ifndef CD_VERSION
    PlayMidi(sound_data[22], sound_sizes[22]);
#endif
}
