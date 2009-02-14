/****************************************************************************
 
    sound.h
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


#ifndef _SOUND_H
#define _SOUND_H



/* sound initializations */
extern void sound_init(void);

/* sound deinitializations */
extern void sound_close(void);


/* sound play */
extern void sound_play(int index);


/* play the main music track */
extern void play_music(void);

/* play music if spell was performed */
extern void music_spell_performed(void);

/* play music if spell failed */
extern void music_spell_failed(void);

/* play music if combat started */
extern void music_combat_started(void);

/* play music if combat was won */
extern void music_combat_won(void);

/* starts stop playing the music track */
extern void toggle_music(void);




#endif /* _SOUND_H */
