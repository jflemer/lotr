/****************************************************************************

    midi.h
    midi music support
    inspiration taken from exult


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


#ifndef _MIDI_H
#define _MIDI_H

#include "lord.h"

/* stops music return nonzero if music was not playing*/
extern int StopMidi(void);


/* start music */
extern void PlayMidi(Uint8 * data, int size);




#endif /* _MIDI_H */
