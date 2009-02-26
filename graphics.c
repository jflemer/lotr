/****************************************************************************

    graphics.c
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

#include "lotr.h"
#include "lotr_sdl.h"
#include "utils.h"
#include "archive.h"
#include "graphics.h"
#include <stdlib.h>
#include <string.h>



/* memory image of the screen */
Uint8 main_screen[SCREEN_WIDTH * SCREEN_HEIGHT];


/* emulation of different graphics modes (VGA,EGA,CGA) */
/* only VGA works */
int gr_mode = VGA_MODE;

/* main screen palette */
Uint8 main_palette[0x300];


/* window parameters */
int window_x = 0, window_y = 0, window_w = SCREEN_WIDTH, window_h =
    SCREEN_HEIGHT;





/*
  reads a pixmap from idx archive
*/

Pixmap *
pixmap_read_from_idx_archive(Archive *archive, int index)
{
    Uint8 *compressed;
    Uint8 *data;
    Pixmap *result;
    int w, h, size;

    compressed = archive_read(archive, index);
    if (compressed == NULL) {
        fprintf(stderr,
                "lotr: could not find pixmap at index %d in the archive\n",
                index);
        exit(1);
    }

    w = compressed[1] * 0x100 + compressed[0];
    h = compressed[3] * 0x100 + compressed[2];

    data =
        decompress_idx(compressed + 4, archive_data_size(archive, index),
                       &size);
    free(compressed);

    if (size != w * h) {
        fprintf(stderr,
                "lotr: corrupted pixmap at index %d in the archive\n", index);
        exit(1);
    }

    result = lotr_malloc(sizeof(Pixmap));

    result->width = w;
    result->height = h;
    result->datasize = size;
    result->hasalpha = 0;
    result->data = data;

    return result;
}






/*
  reads a pixmap from ndx archive
*/

Pixmap *
pixmap_read_from_ndx_archive(Archive *archive, int index)
{
    Uint8 *compressed;
    Uint8 *data;
    Pixmap *result;
    int size;

    compressed = archive_read(archive, index);
    if (compressed == NULL) {
        fprintf(stderr,
                "lotr: could not find pixmap at index %d in the archive\n",
                index);
        exit(1);
    }

    data =
        decompress_ndx(compressed, archive_data_size(archive, index), &size);
    free(compressed);

    result = lotr_malloc(sizeof(Pixmap));

    result->width = 1;
    result->height = size;
    result->datasize = size;
    result->hasalpha = 0;
    result->data = data;

    return result;
}





/*
  creates pixmap
*/

Pixmap *
pixmap_new(int width, int height)
{
    Pixmap *result;

    result = lotr_malloc(sizeof(Pixmap));

    result->width = width;
    result->height = height;
    result->datasize = width * height;
    result->hasalpha = 0;
    result->data = lotr_malloc(width * height);
    bzero(result->data, width * height);

    return result;
}





/*
  frees pixmap
*/

void
pixmap_free(Pixmap *pixmap)
{
    free(pixmap->data);
    free(pixmap);
}




/*
  draws a pixmap to the screen
*/

void
pixmap_draw(Pixmap *pixmap, int x, int y)
{
    pixmap_draw_to_buffer(main_screen, pixmap, x, y);
}


/*
  draws a pixmap to a buffer
*/

void
pixmap_draw_to_buffer(Uint8 *buffer, Pixmap *pixmap, int x, int y)
{
    Uint8 *data = pixmap->data;
    Uint8 *s;
    int w = pixmap->width;
    int h = pixmap->height;
    int i, j;

    if (x + w < 0)
        return;
    if (x < 0) {
        data -= x;
        w += x;
        x = 0;
    }
    if (x >= window_w)
        return;
    if (x + w > window_w)
        w = window_w - x;

    if (y + h < 0)
        return;
    if (y < 0) {
        data -= y * pixmap->width;
        h += y;
        y = 0;
    }
    if (y >= window_h)
        return;
    if (y + h > window_h)
        h = window_h - y;

    s = buffer + (window_y + y) * SCREEN_WIDTH + (window_x + x);

    if (pixmap->hasalpha) {
        for (i = 0; i < h; ++i) {
            for (j = 0; j < w; ++j)
                if (data[j] != pixmap->alpha)
                    s[j] = data[j];
            s += SCREEN_WIDTH;
            data += pixmap->width;
        }
    } else {
        for (i = 0; i < h; ++i) {
            memcpy(s, data, w);
            s += SCREEN_WIDTH;
            data += pixmap->width;
        }
    }

}




/*
  set pixmaps alpha channel
*/

void
pixmap_set_alpha(Pixmap *pixmap, Uint8 alpha)
{
    pixmap->hasalpha = 1;
    pixmap->alpha = alpha;
}


/*
  set pixmaps width
*/

void
pixmap_set_width(Pixmap *pixmap, int width)
{

    if ((pixmap->datasize % width) != 0) {
        fprintf(stderr,
                "lotr: can not set width %d to pixmap with datasize=%d\n",
                width, pixmap->datasize);
#ifndef CD_VERSION
        /* TODO remove CDHACK */
        exit(1);
#endif
    }

    pixmap->width = width;
    pixmap->height = pixmap->datasize / width;

}





/*
  creates a subpixmap
*/

Pixmap *
pixmap_subpixmap(Pixmap *pixmap, int startx, int starty, int endx, int endy)
{
    Pixmap *result;
    int w, h;
    int j;

    if (startx < 0)
        startx = 0;
    if (starty < 0)
        starty = 0;
    if (endx > pixmap->width - 1)
        endx = pixmap->width - 1;
    if (endy > pixmap->height - 1)
        endy = pixmap->height - 1;

    w = endx - startx + 1;
    h = endy - starty + 1;


    if (w <= 0 || h <= 0 || startx >= pixmap->width
        || starty >= pixmap->height)
        return pixmap_new(0, 0);

    result = pixmap_new(w, h);

    result->hasalpha = pixmap->hasalpha;
    result->alpha = pixmap->alpha;

    for (j = 0; j < h; ++j)
        memcpy(result->data + j * w,
               pixmap->data + (starty + j) * pixmap->width + startx, w);

    return result;

}




/*
  creates a pixmap from the screen
*/

Pixmap *
pixmap_subscreen(int startx, int starty, int endx, int endy)
{
    Pixmap *screen;
    Pixmap *result;

    screen = lotr_malloc(sizeof(Pixmap));

    screen->width = SCREEN_WIDTH;
    screen->height = SCREEN_HEIGHT;
    screen->datasize = SCREEN_WIDTH * SCREEN_HEIGHT;
    screen->hasalpha = 0;
    screen->data = main_screen;

    result = pixmap_subpixmap(screen, startx, starty, endx, endy);

    free(screen);

    return result;

}




/*
  draws rectangle
*/

void
draw_rectangle(Uint8 c, int x, int y, int xx, int yy)
{
    int i;
    int a, b, l;
    Uint8 *s;


    if (x >= window_w || y > window_h || xx < 0 || yy < 0)
        return;

    if (xx < x) {
        a = xx;
        xx = x;
        x = a;
    }

    if (yy < y) {
        a = y;
        y = y;
        y = a;
    }

    if (x >= window_w || y > window_h || xx < 0 || yy < 0)
        return;



    /* horizontal lines */
    a = max(x, 0);
    b = min(xx, window_w);
    l = b - a;

    /* upper line */
    if (y >= 0) {
        s = main_screen + (window_y + y) * SCREEN_WIDTH + window_x + a;
        for (i = 0; i < l; ++i, ++s)
            *s = c;
    }

    /* lower line */
    if (yy < window_h) {
        s = main_screen + (window_y + yy) * SCREEN_WIDTH + window_x + a;
        for (i = 0; i < l; ++i, ++s)
            *s = c;
    }



    /* vertical lines */
    ++yy;
    a = max(y, 0);
    b = min(yy, window_h);
    l = b - a;

    /* left */
    if (x >= 0) {
        s = main_screen + (window_y + a) * SCREEN_WIDTH + window_x + x;
        for (i = 0; i < l; ++i, s += SCREEN_WIDTH)
            *s = c;
    }


    /* right */
    if (xx < window_w) {
        s = main_screen + (window_y + a) * SCREEN_WIDTH + window_x + xx;
        for (i = 0; i < l; ++i, s += SCREEN_WIDTH)
            *s = c;
    }


}




/*
  reads a cartoon font from an idx archive
*/

CartoonFont *
cartoon_font_read(Archive *archive, int index)
{

    Uint8 *compressed;
    Uint8 *data, *source, *dest;
    CartoonFont *result;
    int size;
    int foreground;
    int background;
    int linesize;
    int height;
    Pixmap **characters;
    int i, j, k, ch, chx, c;


    compressed = archive_read(archive, index);
    if (compressed == NULL) {
        fprintf(stderr,
                "lotr: could not find font at index %d in the archive\n",
                index);
        exit(1);
    }

    data =
        decompress_idx(compressed, archive_data_size(archive, index), &size);
    free(compressed);


    result = lotr_malloc(sizeof(CartoonFont));

    foreground = 1;
    background = 0;
    height = data[0];
    result->charnum = data[1];
    result->firstchar = data[2];
    /* unknown data[3]; */
    linesize = 0x100 * data[5] + data[4];


    /* I do not know what is stored in the last 144 bytes */
    if (size != 6 + result->charnum + height * linesize + 144) {

        fprintf(stderr, "lotr: corrupted font at index %d in the archive\n",
                index);
        exit(1);
    }

    characters = lotr_malloc(result->charnum * sizeof(Pixmap *));
    result->characters = characters;

    if (result->charnum == 0) {
        free(data);
        return result;
    }

    for (i = 0; i < result->charnum; ++i) {
        characters[i] = pixmap_new(data[6 + i], height);
        pixmap_set_alpha(characters[i], background);
    }


    for (j = 0; j < height; ++j) {
        /* pointer to the beginning of the line */
        source = data + 6 + result->charnum + linesize * j;

        ch = 0;                 /* the character we are writing to */
        chx = 0;                /* x position in this character */
        dest = characters[0]->data + j * characters[0]->width;
        for (i = 0; i < linesize && ch < result->charnum; ++i) {
            c = source[i];
            for (k = 0; k < 8; ++k) {   /* cycle thru all bits */

                if (c & 0x80)
                    *(dest++) = foreground;
                else
                    *(dest++) = background;

                ++chx;

                if (chx == characters[ch]->width) {
                    /* we have finished this charactrs line */
                    ch++;
                    if (ch == result->charnum)
                        break;

                    chx = 0;
                    dest = characters[ch]->data + j * characters[ch]->width;
                }

                c *= 2;
            }
        }
    }

    free(data);

    return result;
}


/*
  frees cartoon font
*/

void
cartoon_font_free(CartoonFont *font)
{
    int i;
    for (i = 0; i < font->charnum; ++i)
        pixmap_free(font->characters[i]);
    free(font);
}



/*
  writes text
*/

void
cartoon_font_write_text(CartoonFont *font, int x, int y, char *text)
{

    int ch;

    while ((ch = *text) != 0) {
        ch -= font->firstchar;
        if (0 <= ch && ch < font->charnum) {
            pixmap_draw(font->characters[ch], x, y);
            x += font->characters[ch]->width;
        }
        text++;
    }
}





/*
  updates a screen
*/

void
graphics_update_screen(void)
{
    lotr_show_screen(main_screen);
}



/*
  clears the screen
*/

void
graphics_clear_screen(void)
{
    graphics_set_screen(0);
}


/*
  sets the screen color
*/

void
graphics_set_screen(int color)
{
    Uint8 *s;
    int j;
    s = main_screen + window_x + window_y * SCREEN_WIDTH;
    for (j = 0; j < window_h; ++j) {
        memset(s, color, window_w);
        s += SCREEN_WIDTH;
    }
}


/*
  shows a background image
*/
void
graphics_set_background(char *name)
{
    Palette palette;
    FILE *pal;
    FILE *dat;
    char palname[20];
    char datname[20];
    int data_size;
    int picture_size;
    Uint8 *data;
    Uint8 *picture_data;
    Pixmap *background;

    sprintf(palname, "%s.pal", name);
    pal = lotr_fopen(palname, "rb");
    sprintf(datname, "%s.dat", name);
    dat = lotr_fopen(datname, "rb");

    fread(&palette, sizeof(Palette), 1, pal);

    data_size = lotr_filelen(dat);
    data = lotr_malloc(data_size);

    fread(data, 1, data_size, dat);

    picture_data = decompress_ndx(data, data_size, &picture_size);

    if (picture_size != 64000) {
        fprintf(stderr, "lotr: wrong background picture\n");
        exit(1);
    }

    background = pixmap_new(320, 200);
    free(background->data);
    background->data = picture_data;

    fclose(pal);
    fclose(dat);
    free(data);

    graphics_set_palette(&palette, 0, 0x100);

    pixmap_draw(background, 0, 0);

    pixmap_free(background);

    graphics_update_screen();
}



/*
  saves the screen
*/

void
graphics_save_screen(Uint8 *backupscreen)
{
    memcpy(backupscreen, main_screen, SCREEN_WIDTH * SCREEN_HEIGHT);
}

/*
  loads the screen
*/

void
graphics_load_screen(Uint8 *backupscreen)
{
    memcpy(main_screen, backupscreen, SCREEN_WIDTH * SCREEN_HEIGHT);
}



/*
  clears screen area
*/
void
graphics_clear_area(int startx, int starty, int endx, int endy)
{

    Pixmap *pixmap;

    if (endx < startx)
        return;
    if (endy < starty)
        return;

    pixmap = pixmap_new(endx - startx + 1, endy - starty + 1);

    bzero(pixmap->data, pixmap->width * pixmap->height);

    pixmap_draw(pixmap, startx, starty);

    pixmap_free(pixmap);

}



/*
  sets the display window
*/

void
graphics_set_window(int startx, int starty, int endx, int endy)
{

    window_x = startx;
    window_y = starty;

    if (window_x < 0)
        window_x = 0;
    if (window_x >= SCREEN_WIDTH - 1) {
        window_w = 0;
        window_h = 0;
        return;
    }
    if (endx >= SCREEN_WIDTH - 1)
        endx = SCREEN_WIDTH - 1;

    if (window_y < 0)
        window_y = 0;
    if (window_y >= SCREEN_HEIGHT - 1) {
        window_w = 0;
        window_h = 0;
        return;
    }
    if (endy >= SCREEN_HEIGHT - 1)
        endy = SCREEN_HEIGHT - 1;

    window_w = endx - startx + 1;
    window_h = endy - starty + 1;

    if (window_w < 0 || window_h < 0) {
        window_w = 0;
        window_h = 0;
    }

}




/*
  rotates num colors starting at start
 */

void
graphics_rotate_palette_right(int start, int end)
{

    Uint8 r, g, b;
    int num;

    num = end - start + 1;

    if (num < 2)
        return;

    /* stores the last color */
    r = main_palette[(start + num - 1) * 3 + 0];
    g = main_palette[(start + num - 1) * 3 + 1];
    b = main_palette[(start + num - 1) * 3 + 2];

    /* shifts the colors */
    memmove(main_palette + start * 3 + 3, main_palette + start * 3,
            (num - 1) * 3);

    /* writes the stored color on the first place */
    main_palette[start * 3 + 0] = r;
    main_palette[start * 3 + 1] = g;
    main_palette[start * 3 + 2] = b;

    lotr_system_set_palette(main_palette, start, num);

}



/*
  rotates num colors starting at start - do not display
 */

void
graphics_rotate_palette_leftHidden(int start, int end)
{

    Uint8 r, g, b;
    int num;

    num = end - start + 1;

    if (num < 2)
        return;

    /* stores the first color */
    r = main_palette[start * 3 + 0];
    g = main_palette[start * 3 + 1];
    b = main_palette[start * 3 + 2];

    /* shifts the colors */
    memmove(main_palette + start * 3, main_palette + start * 3 + 3,
            (num - 1) * 3);

    /* writes the stored color on the last place */
    main_palette[(start + num - 1) * 3 + 0] = r;
    main_palette[(start + num - 1) * 3 + 1] = g;
    main_palette[(start + num - 1) * 3 + 2] = b;

}


/*
  rotates num colors starting at start
 */

void
graphics_rotate_palette_left(int start, int end)
{
    int num;

    num = end - start + 1;

    if (num < 2)
        return;

    graphics_rotate_palette_leftHidden(start, end);
    lotr_system_set_palette(main_palette, start, num);
}



/*
  sets palette
*/

void
graphics_set_palette(Palette *palette, int firstcolor, int ncolors)
{

    int i, c;

    if (firstcolor < 0 || firstcolor + ncolors > 256) {
        fprintf(stderr, "lotr: wrong graphics_set_palette parameters\n");
        exit(1);
    }

    if (gr_mode == VGA_MODE) {
        memcpy(main_palette + 3 * firstcolor,
               palette->colors + 3 * firstcolor, 3 * ncolors);
    }

    /* this does not work! */
    if (gr_mode == EGA_MODE) {
        for (i = 0; i < ncolors; ++i) {
            c = palette->egacolors[palette->egamapping[firstcolor + i]];
            main_palette[3 * firstcolor + 3 * i + 0] =
                palette->colors[3 * c + 0];
            main_palette[3 * firstcolor + 3 * i + 1] =
                palette->colors[3 * c + 1];
            main_palette[3 * firstcolor + 3 * i + 2] =
                palette->colors[3 * c + 2];
        }
    }

    lotr_system_set_palette(main_palette, firstcolor, ncolors);

}





/*
  fades palette
*/

void
graphics_fade_palette(int coef, int firstcolor, int ncolors)
{

    Uint8 faded[0x300];
    Uint8 *f, *p;
    int i;

    f = faded + firstcolor * 3;
    p = main_palette + firstcolor * 3;
    for (i = 0; i < 3 * ncolors; ++i)
        *(f++) = (Uint8)(*(p++)) * coef / 100;




    lotr_system_set_palette(faded, firstcolor, ncolors);

}




/*
  copies palette colors
*/

void
palette_copy_colors(Palette *palette, int start, int num, int newstart)
{
    if (start < 0 || num < 0 || start + num > 0xff || newstart < 0
        || newstart + num > 0xff) {
        fprintf(stderr, "lotr: wrong palette_reindex parameters\n");
        exit(1);
    }

    memmove(palette->colors + 3 * newstart, palette->colors + 3 * start,
            3 * num);
    memmove(palette->egamapping + newstart, palette->egamapping + start, num);
    memmove(palette->cgamapping + newstart, palette->cgamapping + start, num);


}
