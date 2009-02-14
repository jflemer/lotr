/****************************************************************************
 
    graphics.h
    graphics manipulation


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


#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "lord.h"
#include "archive.h"



typedef struct
{
    int width;
    int height;
    int datasize;
    int hasalpha;
    Uint8 alpha;
    Uint8 *data;
} Pixmap;


typedef struct
{
    int charnum;                /* number of characters */
    int firstchar;              /* first character */
    Pixmap **characters;
} CartoonFont;




typedef struct
{
    Uint8 colors[0x300];        /* colors in rgb format */
    Uint8 egacolors[0x10];      /* mapping of 16 ega colors to vga colors */
    Uint8 egamapping[0x100];    /* mapping of 256 vga colors to egacolors */
    Uint8 unknown[2];
    Uint8 cgamapping[0x100];    /* mapping of 256 vga colors to cga colors */
} Palette;



/* emulation of different graphics modes */
#define VGA_MODE 0
#define EGA_MODE 1
#define CGA_MODE 2


/* reads a pixmap from idx archive */
extern Pixmap *pixmap_read_from_idxarchive(Archive * archive, int index);

/* reads a pixmap from ndx archive */
extern Pixmap *pixmap_read_from_ndxarchive(Archive * archive, int index);

/* draws a pixmap to the screen */
extern void pixmap_draw(Pixmap * pixmap, int x, int y);

/* draws a pixmap to a buffer */
extern void pixmap_drawtobuffer(Uint8 * buffer, Pixmap * pixmap, int x,
                                int y);

/* frees pixmap */
extern void pixmap_free(Pixmap * pixmap);

/* creates pixmap */
extern Pixmap *pixmap_new(int width, int height);

/* set pixmaps alpha channel */
extern void pixmap_setalpha(Pixmap * pixmap, Uint8 alpha);

/* set pixmaps width */
extern void pixmap_setwidth(Pixmap * pixmap, int width);

/* creates a subpixmap */
extern Pixmap *pixmap_subpixmap(Pixmap * pixmap, int startx, int starty,
                                int endx, int endy);

/* creates a pixmap from the screen */
extern Pixmap *pixmap_subscreen(int startx, int starty, int endx, int endy);

/* creates a pixmap from the screen */
extern Pixmap *pixmap_subscreen(int startx, int starty, int endx, int endy);



/* draws rectangle */
extern void draw_rectangle(Uint8 c, int x, int y, int xx, int yy);


/* reads a cartoon font from an idx archive */
extern CartoonFont *cartoonfont_read(Archive * archive, int index);

/* frees cartoon font */
extern void cartoonfont_free(CartoonFont * font);

/* writes text */
extern void cartoonfont_writetext(CartoonFont * font, int x, int y,
                                  char *text);




/* updates the screen */
extern void UpdateScreen(void);

/* clears the screen */
extern void ClearScreen(void);

/* sets the screen color*/
extern void SetScreen(int color);

/* shows a background image */
extern void SetBackground(char *name);

/* saves the screen */
extern void SaveScreen(Uint8 * backupscreen);

/* loads the screen */
extern void LoadScreen(Uint8 * backupscreen);

/* clears screen area */
extern void ClearArea(int startx, int starty, int endx, int endy);

/* sets the display window */
extern void SetWindow(int startx, int starty, int endx, int endy);



/* rotates num colors starting at start */
extern void RotatePaletteRight(int start, int end);

/* rotates num colors starting at start */
extern void RotatePaletteLeft(int start, int end);

/* rotates num colors starting at start - do not display */
extern void RotatePaletteLeftHidden(int start, int end);

/* sets palette */
extern void SetPalette(Palette * palette, int firstcolor, int ncolors);

/* fades palette */
extern void FadePalette(int coef, int firstcolor, int ncolors);

/* copies palette colors */
extern void palette_copy_colors(Palette * palette, int start, int num,
                                int newstart);


#endif /* _GRAPHICS_H */
