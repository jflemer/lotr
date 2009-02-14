/****************************************************************************

    lord_sdl.h
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


#include <SDL_keysym.h>


#define KEY_ENTER       SDLK_RETURN
#define KEY_BACKSPACE   SDLK_BACKSPACE



/* initialize SDL */
extern void System_Init(void);

/* quit SDL */
extern void System_Close(void);


/* play sound sample */
extern void PlaySample(Uint8 * data, int size);

/* resize sound sample */
extern void ResizeSample(Uint8 * data, int size);

/* are we playing a sound? */
extern int PlayingSample(void);


/* polls all pending events */
extern void PollEvents(void);

/* resets keyboard status */
extern void ResetKeyboard(void);

/* was a key pressed? */
extern int KbHit(void);

/* get last pressed key - return 0 if none */
extern int GetKey(void);

/* disable keyboard input */
extern void InputDisable(void);

/* enable keyboard input */
extern void InputEnable(void);



/* return key states */
extern int KeyLeft();
extern int KeyRight();
extern int KeyUp();
extern int KeyDown();
extern int KeyEsc();
extern int KeyShift();
extern int KeyCtrl();




/* shows a new screen */
extern void ShowScreen(Uint8 * newscreen);


/* sets palette */
extern void SystemSetPalette(Uint8 * palette, int firstcolor, int ncolors);




#endif /* _LORD_SDL_H */
