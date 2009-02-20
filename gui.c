const /****************************************************************************

    gui.c
    graphics user interface


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
#include "game.h"
#include "cartoon.h"
#include "character.h"
#include "combat.h"
#include "gui.h"
#include "graphics.h"
#include "lord_sdl.h"
#include "map.h"
#include "object.h"
#include "shape.h"
#include "sound.h"
#include "timing.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef EXTENDED
#include "pythonspot.h"
#endif



Character *choosed_character;

int wizard_mode;
char wizard_text[1024];

#define PARAGRAPHS_NUM 259
char *paragraphs;
int paragraphs_off[PARAGRAPHS_NUM + 1];


int dialog_mode = DIALOG_NONE;
int dialog_confirm;


int gui_menu_x = -1;
int gui_menu_y = -1;

int gui_paragraph_pos;
int gui_paragraph_isquestion;


/* should we show spots? */
#ifdef DRAW_SPOTS
int gui_show_spots = 1;
#else
int gui_show_spots = 0;
#endif

Palette *gui_palette;


/* various GUI components */
#define TEST 15
#define GUI_CHAIN_HORIZ   0
#define GUI_MOUSE         1
#define GUI_MENU          2
#define GUI_SCROLL_TOP    3
#define GUI_CHAIN_VERT    4
#define GUI_CHAIN_SW      5
#define GUI_SCROLL_SW     6
#define GUI_CHAIN_SE      7
#define GUI_SCROLL_SE     8
#define GUI_SCROLL_EMPTY  9
#define GUI_SCROLL_BOT   10
#define GUI_CHAIN_NW     11
#define GUI_SCROLL_NW    12
#define GUI_CHAIN_NE     13
#define GUI_SCROLL_NE    14
#define GUI_FONT         15
#define GUI_COMP_NUM     16

Pixmap *gui_components[GUI_COMP_NUM];
int gui_comp_widths[GUI_COMP_NUM] = { /*00 */ 5,
    /*01 */ 160,
    /*02 */ 320,
    /*03 */ 8,
    /*04 */ 6,
    /*05 */ 10,
    /*06 */ 8,
    /*07 */ 10,
    /*08 */ 8,
    /*09 */ 8,
    /*10 */ 8,
    /*11 */ 10,
    /*12 */ 8,
    /*13 */ 10,
    /*14 */ 8,
    /*15 */ 256
};

#define DEFAULT_SCROLL_WIDTH 20

#define SCROLL_ELEMENT_SIZE 8

Pixmap *gui_font[0x100];

Pixmap *gui_life_bar;

/*
  init gui
*/

void
gui_init(void)
{
    Archive *archive;
    FILE *pal;
    int i, j;


    /* menu mode initializations */
    dialog_mode = DIALOG_NONE;
    gui_menu_x = gui_menu_y = 0;


    for (i = 0; i < GUI_COMP_NUM; ++i)
        gui_components[i] = NULL;

    for (i = 0; i < 0x100; ++i)
        gui_font[i] = NULL;

    pal = lord_fopen("arts.pal", "rb");
    if (lord_filelen(pal) != sizeof(Palette)) {
        fprintf(stderr, "lord: arts.pal is not a valid palette file\n");
        exit(1);
    }

    gui_palette = lord_malloc(sizeof(Palette));

    if (fread(gui_palette, sizeof(Palette), 1, pal) != 1) {
        fprintf(stderr, "lord: can not read arts.pal\n");
        exit(1);
    }

    fclose(pal);

    /* arts palette is stored in 0..0x19 and is mapped to 0x40..0x59 */

    palette_copy_colors(gui_palette, 0, 0x20, 0x40);
    gui_set_palette();


    archive = archive_ndx_open("arts");

    for (i = 0; i < GUI_COMP_NUM; ++i) {
        gui_components[i] = pixmap_read_from_ndx_archive(archive, i);
        pixmap_set_width(gui_components[i], gui_comp_widths[i]);
    }

    archive_close(archive);

    for (j = 0; j < 3; ++j)
        for (i = 0; i < 0x20; ++i)
            gui_font[0x20 + j * 0x20 + i] =
                pixmap_subpixmap(gui_components[GUI_FONT], i * 8, j * 8,
                                 i * 8 + 7, j * 8 + 7);


    gui_life_bar = pixmap_new(10, 100);

    memset(gui_life_bar->data, 0x4, gui_life_bar->datasize);


    /* init paragraphs */

    if (lord_file_exists("para.dat")) {
        FILE *parfile;
        int parlord_filelen;
        int pos, n;
        parfile = lord_fopen("para.dat", "rb");
        parlord_filelen = lord_filelen(parfile);
        paragraphs = lord_malloc(parlord_filelen);
        if (fread(paragraphs, 1, parlord_filelen, parfile) != parlord_filelen) {
            fprintf(stderr, "lord: can not read file para.dat\n");
            perror("lord");
            exit(1);
        }

        pos = 0;
        n = 0;
        while (pos < parlord_filelen && n <= PARAGRAPHS_NUM) {
            paragraphs_off[n] = pos;
            while (pos < parlord_filelen && paragraphs[pos] != 0x10)
                ++pos;
            pos += 3;
            ++n;
        }

        while (n <= PARAGRAPHS_NUM) {
            paragraphs_off[n] = 0;
            ++n;
        }

        fclose(parfile);
    }
    else {
        paragraphs = NULL;
    }

}




/*
  init close
*/

void
gui_close(void)
{
    int i;

    for (i = 0; i < GUI_COMP_NUM; ++i)
        if (gui_components[i])
            pixmap_free(gui_components[i]);

    for (i = 0; i < 0x100; ++i)
        if (gui_font[i])
            pixmap_free(gui_font[i]);

    free(gui_palette);

    pixmap_free(gui_life_bar);

    if (paragraphs != NULL)
        free(paragraphs);

}





/*
  set the gui palette
*/

void
gui_set_palette(void)
{
    graphics_set_palette(gui_palette, 0x40, 0x20);
}



/*
  clears screen and draws the bounding chain
*/

void
gui_clear(void)
{
    int i;
    Pixmap *pixmap;


    graphics_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    graphics_clear_screen();

    gui_set_full_window();


    /* horizontal lines */
    pixmap = gui_components[GUI_CHAIN_HORIZ];
    for (i = gui_components[GUI_CHAIN_NW]->width;
         i < SCREEN_WIDTH - gui_components[GUI_CHAIN_NE]->width;
         i += pixmap->width) {
        pixmap_draw(pixmap, i, 1);
        pixmap_draw(pixmap, i, SCREEN_HEIGHT - pixmap->height);
    }


    /* vertical lines */
    pixmap = gui_components[GUI_CHAIN_VERT];
    for (i = gui_components[GUI_CHAIN_NW]->height;
         i < SCREEN_HEIGHT - gui_components[GUI_CHAIN_SW]->height;
         i += pixmap->height) {
        pixmap_draw(pixmap, 0, i);
        pixmap_draw(pixmap, SCREEN_WIDTH - pixmap->width - 1, i);
    }



    /* screen corners */
    pixmap = gui_components[GUI_CHAIN_NW];
    pixmap_draw(pixmap, 0, 0);

    pixmap = gui_components[GUI_CHAIN_NE];
    pixmap_draw(pixmap, SCREEN_WIDTH - pixmap->width, 0);

    pixmap = gui_components[GUI_CHAIN_SW];
    pixmap_draw(pixmap, 0, SCREEN_HEIGHT - pixmap->height);

    pixmap = gui_components[GUI_CHAIN_SE];
    pixmap_draw(pixmap, SCREEN_WIDTH - pixmap->width,
                SCREEN_HEIGHT - pixmap->height);

}



/*
  draws scroll with a text
*/

void
draw_scroll(int x, int y, int width, int height, const char *text[])
{
    /* size of one scroll element */
    int size = SCROLL_ELEMENT_SIZE;

    int i, j;

    /* if width<0 draw the scroll in SE corner */


    if (width < 0) {
        width = -width;
        x = SCREEN_WIDTH - 8 - size * (2 + width);
        y = SCREEN_HEIGHT - size * (2 + height);
    }

    /* first draw an empty scroll */

    for (i = 0; i < width; ++i) {
        pixmap_draw(gui_components[GUI_SCROLL_TOP], x + (i + 1) * size, y);
        for (j = 0; j < height; ++j)
            pixmap_draw(gui_components[GUI_SCROLL_EMPTY],
                        x + (i + 1) * size, y + (j + 1) * size);
        pixmap_draw(gui_components[GUI_SCROLL_BOT],
                    x + (i + 1) * size, y + (height + 1) * size);
    }

    pixmap_draw(gui_components[GUI_SCROLL_NW], x, y);
    pixmap_draw(gui_components[GUI_SCROLL_SW], x, y + (height + 1) * size);
    pixmap_draw(gui_components[GUI_SCROLL_NE], x + (width + 1) * size, y);
    pixmap_draw(gui_components[GUI_SCROLL_SE], x + (width + 1) * size,
                y + (height + 1) * size);

    /* next draw the text */
    for (j = 0; j < height; ++j)
        for (i = 0; i < width; ++i) {
            if (gui_font[(Uint8)text[j][i]])
                pixmap_draw(gui_font[(Uint8)text[j][i]],
                            x + (i + 1) * size, y + (j + 1) * size);
            if (text[j][i] == 0)
                break;
        }


}








/*
  set the map drawing area
*/

void
gui_set_map_area(int *width, int *height)
{


    if (dialog_mode < MAIN_MENU
        || SCREEN_WIDTH > 320 /* Draw whole map for large screens */ ) {

#if PIXEL_PRECISE

        *width = SCREEN_WIDTH - 16;
        *height = SCREEN_HEIGHT - 16;

        graphics_set_window(8, 8, 8 + *width - 1, 8 + *height - 1);

#else

        *width = SCREEN_WIDTH - 17;
        *height = SCREEN_HEIGHT - 17;

        graphics_set_window(8, 9, 8 + *width - 1, 9 + *height - 1);

#endif
    }
    else {
#if PIXEL_PRECISE

        *width = SCREEN_WIDTH - 16;
        *height = SCREEN_HEIGHT - 8 - gui_components[GUI_MENU]->height;

        graphics_set_window(8, 8, 8 + *width - 1, 8 + *height - 1);

#else

        *width = SCREEN_WIDTH - 17;
        *height = SCREEN_HEIGHT - 8 - gui_components[GUI_MENU]->height - 2;

        graphics_set_window(8, 9, 8 + *width - 1, 9 + *height - 1);

#endif
    }

}




/*
  set the window over whole screen
*/

void
gui_set_full_window(void)
{
#if PIXEL_PRECISE
    graphics_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 2);
#else
    graphics_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
#endif
}




/*
  draws main menu
 */


#define MAX_LIFE_BAR 55

void
main_menu_draw(void)
{
    int life;

    /* Keep x position from standard screen size */
    gui_menu_x = 0;
    gui_menu_y = SCREEN_HEIGHT - gui_components[GUI_MENU]->height;

    pixmap_draw(gui_components[GUI_MENU], gui_menu_x, gui_menu_y);

    portrait_draw(choosed_character->portrait, gui_menu_x + 48,
                  gui_menu_y + 9);

    life = MAX_LIFE_BAR * choosed_character->life / choosed_character->end;
    if (life > MAX_LIFE_BAR)
        life = MAX_LIFE_BAR;
    gui_life_bar->height = life;
    pixmap_draw(gui_life_bar, gui_menu_x + 20,
                gui_menu_y + 9 + MAX_LIFE_BAR - life);
    gui_life_bar->height = 100;
}





/*
  show main menu
 */


void
main_menu_show(void)
{
    dialog_mode = MAIN_MENU;

    if (dialog_mode == DIALOG_NONE && !combat_get_mode())
        choosed_character = game_get_leader();

    if (!choosed_character)
        choosed_character = game_get_leader();

    gui_clear();
    game_draw_map();

    main_menu_draw();
}





/*
  quits all menus
*/

void
quit_menu()
{
    dialog_mode = DIALOG_NONE;
    choosed_character = NULL;

    if (combat_get_mode()) {
        main_menu_show();
    }
    else {
        gui_clear();
        game_draw_map();
        lord_reset_keyboard();
    }
}



#define MAX_MESSAGELINES 256


/*
  format message text
 */

char *
format_text(char *_text, int *lines, int width, const char *formatted_text[])
{
    int n;
    char *text, *t;
    int i, j, line;

    n = strlen(_text);
    t = text = lord_malloc(n + 1);
    strncpy(text, _text, n);
    text[n] = 0;

    /* format text */

    line = 0;
    i = j = 0;

    while (t[j] != 0) {
        if (t[j] == ' ')
            i = j;
        if (j == width && i > 0) {
            t[i] = 0;
            formatted_text[line++] = t;
            t = t + i + 1;
            i = j = 0;
        }
        ++j;
    }


    if (j != 0) {
        t[j] = 0;
        formatted_text[line++] = t;
    }

    if (line == 1) {
        formatted_text[line++] = t + j;
    }

    *lines = line;
    return text;

}


/*
  shows a message
 */

void
gui_message(char *text, int small_window)
{
    const char *formatted_text[MAX_MESSAGELINES];
    int width;
    int y;
    char *tmptext;
    int lines;


    if (text == NULL)
        return;

    if (small_window) {
        main_menu_show();
        width = DEFAULT_SCROLL_WIDTH;
    }
    else {
        if (dialog_mode != DIALOG_BOOK) {
            gui_clear();
            game_draw_map();
        }
        width = SCREEN_WIDTH / SCROLL_ELEMENT_SIZE - 4;
    }

    if (dialog_mode != DIALOG_BOOK)
        dialog_mode = DIALOG_MESSAGE;

    tmptext = format_text(text, &lines, width, formatted_text);

    y = 128;
    if (lines > 6)
        y -= SCROLL_ELEMENT_SIZE * (lines - 6);
    if (y < SCROLL_ELEMENT_SIZE)
        y = SCROLL_ELEMENT_SIZE;

    if (small_window) {
        while (lines < 7)
            formatted_text[lines++] = "";
        draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, 7, formatted_text);
    }
    else {
        draw_scroll(SCROLL_ELEMENT_SIZE, 128, width, lines, formatted_text);
    }

    free(tmptext);

}


/*
  parse message keyboard input
*/


void
dialog_message_key(int key)
{
    if (key == KEY_ENTER || key == ' ' || key == 'x') {
#ifdef TTT
        if (dialog_mode == DIALOG_BOOK) {
            gui_set_palette();
            map_set_palette();
            shapes_set_palette();
            gui_clear();
        }
#endif
        quit_menu();
    }

}



/*
  shows a question
 */


void
gui_question(char *text)
{
    gui_message(text, 0);
    lord_reset_keyboard();
    dialog_mode = DIALOG_MESSAGE_YN;
}



/*
  parse question keyboard input
*/


void
dialog_message_yn_key(int key)
{
    if (key == 'n') {
        spot_if_result(game_get_actual_spot(), 0);
        quit_menu();
    }
    if (key == 'y') {
        spot_if_result(game_get_actual_spot(), 1);
        quit_menu();
    }
}



/*
  shows a help message
 */

void
gui_help_show()
{
    const char *help_text[] = {
        "",
        " Arrows - move character",
        " Space  - game menu",
        " Escape - exit/options menu",
#ifndef DEBUG
        " Shift  - 4x time compression",
#endif
        "",
        " A  Attack        O  Options",
        " C  Choose char.  Q  Quit game",
        " G  Get/Buy       T  Talk",
        " H  Help          S  Skill",
        " L  Set leader    U  Use",
        " M  Magic         V  View char. ",
#ifdef DEBUG
        "",
        " Cheating:",
        " W  Wizard mode",
        " Ctrl+Arrows  Noclip mode",
        " Shift+Arrows Fast noclip",
#endif
        ""
    };
    int lines = sizeof(help_text) / sizeof(const char *);
    int width = 0;
    int xoff, yoff;
    int i;

    for (i = 0; i < lines; ++i) {
        int len = strlen(help_text[i]);
        width = max(len, width);
    }

    xoff = (SCREEN_WIDTH - (width + 2) * SCROLL_ELEMENT_SIZE) / 2;
    yoff = (SCREEN_HEIGHT - (lines + 2) * SCROLL_ELEMENT_SIZE) / 2;

    dialog_mode = DIALOG_MESSAGE;

    draw_scroll(xoff, yoff, width, lines, help_text);
}


/*
  draws gui paragraph scroll
*/

void
gui_paragraph_scroll(void)
{
    const char *formatted_text[MAX_MESSAGELINES];
    int lines, width, y;

    gui_clear();
    game_draw_map();
    width = SCREEN_WIDTH / SCROLL_ELEMENT_SIZE - 10;

    if (paragraphs[gui_paragraph_pos] == 0x5)
        ++gui_paragraph_pos;

    lines = 0;
    while (paragraphs[gui_paragraph_pos] != 0x10 &&
           paragraphs[gui_paragraph_pos] != 0x5 && lines < MAX_MESSAGELINES) {
        /* add one line */
        formatted_text[lines] = paragraphs + gui_paragraph_pos;

        while (paragraphs[gui_paragraph_pos] != 0x0d &&
               paragraphs[gui_paragraph_pos] != 0)
            ++gui_paragraph_pos;
        paragraphs[gui_paragraph_pos] = 0;

        gui_paragraph_pos += 2;
        lines++;
    }

    y = SCREEN_HEIGHT - (lines + 2) * SCROLL_ELEMENT_SIZE;
    if (y > 128)
        y = 128;
    if (y < 0)
        y = 0;

    draw_scroll(SCROLL_ELEMENT_SIZE * 4, y, width, lines, formatted_text);
}



/*
  shows a paragraph
*/

void
gui_paragraph(int num)
{

    if (paragraphs == NULL) {
        gui_message(game_get_paragraph_text(num, 0), 0);
        return;
    }


    if (num < 0 || num > PARAGRAPHS_NUM)
        num = 0;

    gui_paragraph_isquestion = 0;
    gui_paragraph_pos = paragraphs_off[num];

    gui_paragraph_scroll();

    if (paragraphs[gui_paragraph_pos] == 0x10) {
        /* paragraph does not continue */
        dialog_mode = DIALOG_MESSAGE;
    }
    else {
        /* paragraph continues */
        dialog_mode = DIALOG_PARAGRAPH;
    }

}



/*
  shows a question paragraph
*/

void
gui_paragraph_question(int num)
{
    if (paragraphs == NULL) {
        gui_question(game_get_paragraph_text(num, 1));
        return;
    }

    gui_paragraph(num);
    gui_paragraph_isquestion = 1;

    if (dialog_mode != DIALOG_PARAGRAPH) {
        lord_reset_keyboard();
        dialog_mode = DIALOG_MESSAGE_YN;
    }
}


/*
  parse paragraph keyboard input
*/

void
dialog_paragraph_key(int key)
{
    if (key == KEY_ENTER || key == ' ' || key == 'x') {
        gui_paragraph_scroll();

        if (paragraphs[gui_paragraph_pos] == 0x10) {
            /* paragraph does not continue */
            if (gui_paragraph_isquestion) {
                lord_reset_keyboard();
                dialog_mode = DIALOG_MESSAGE_YN;
            }
            else
                dialog_mode = DIALOG_MESSAGE;
        }
        else {
            /* paragraph continues */
            dialog_mode = DIALOG_PARAGRAPH;
        }
    }

}

#define MAX_SPOTLINES 4096
static const char *spot_formatted_text[MAX_SPOTLINES];
static int spot_formatted_text_lines = -1;
static int spot_formatted_text_pos = -1;
static int spot_print_xoff = -1;
static int spot_scroll_width = 256;

/*
  show the active spot
*/
void
dialog_print_active_spot()
{
    Character *leader;
    CommandSpot *spot;
    char *spot_string;
    int screen_height = SCREEN_HEIGHT / SCROLL_ELEMENT_SIZE;
    int lines;

    if (dialog_mode != DIALOG_SPOT_PRINT || spot_formatted_text_lines < 0) {
        char *buf;
        int i;

        if ((leader = game_get_leader()) == NULL)
            return;

        if ((spot = map_get_spot(leader)) == NULL)
            return;

        spot_string = spot_get_string(spot);

        dialog_mode = DIALOG_SPOT_PRINT;

        /* Format the code */
        spot_formatted_text_pos = 0;
        spot_formatted_text_lines = 0;
        spot_print_xoff = 0;
        buf = spot_string;

        while (spot_formatted_text_lines < MAX_SPOTLINES && *buf) {
            spot_formatted_text[spot_formatted_text_lines++] = buf;
            while (*buf && *buf != '\n')
                buf++;

            /* new line found */
            if (*buf) {
                *buf = 0;
                buf++;
            }
        }

        spot_scroll_width = SCREEN_WIDTH / SCROLL_ELEMENT_SIZE;
        for (i = 0; i < spot_formatted_text_lines; ++i)
            spot_scroll_width = max(spot_scroll_width, strlen(spot_formatted_text[i]));
    }

    lines =
        min(screen_height, spot_formatted_text_lines - spot_formatted_text_pos);
    draw_scroll(-SCROLL_ELEMENT_SIZE * (spot_print_xoff + 1), -SCROLL_ELEMENT_SIZE, 256, lines,
                spot_formatted_text + spot_formatted_text_pos);
}


/*
  parse spot print key
*/

void
dialog_print_active_spot_key(int key)
{
    int screen_height = SCREEN_HEIGHT / SCROLL_ELEMENT_SIZE;
    int screen_width = SCREEN_WIDTH / SCROLL_ELEMENT_SIZE;
    int pg_size = max(screen_height - 2, 1);
    int last_pos = spot_formatted_text_pos;
    int last_xoff = spot_print_xoff;

    if (key == KEY_ENTER || key == 'x' || key == 'q') {
        spot_formatted_text_lines = -1;
        quit_menu();
    }

    if (key == KEY_UP) {
        spot_formatted_text_pos--;
    }
    else if (key == KEY_DOWN) {
        spot_formatted_text_pos++;
    }
    else if (key == KEY_PAGEUP) {
        spot_formatted_text_pos -= pg_size;
    }
    else if (key == KEY_PAGEDOWN) {
        spot_formatted_text_pos += pg_size;
    }
    else if (key == KEY_LEFT) {
        spot_print_xoff--;
    }
    else if (key == KEY_RIGHT) {
        spot_print_xoff++;
    }

    if (spot_formatted_text_pos + screen_height > spot_formatted_text_lines)
        spot_formatted_text_pos = spot_formatted_text_lines - screen_height;

    if (spot_formatted_text_pos < 0)
        spot_formatted_text_pos = 0;

    if (spot_print_xoff + screen_width > spot_scroll_width)
        spot_print_xoff = spot_scroll_width - screen_width;

    if (spot_print_xoff < 0)
        spot_print_xoff = 0;

    /* Redraw the scroll */
    if (last_pos != spot_formatted_text_pos
        || last_xoff != spot_print_xoff)
    {
        dialog_print_active_spot();
    }
}


#ifdef TTT

/*
  shows a book with the next chapter in TTT
*/

void
gui_book(char *text)
{

    graphics_set_background("back");
    gui_set_palette();

    portrait_draw(game_get_leader()->portrait, 168, 51);

    dialog_mode = DIALOG_BOOK;
    gui_message(text, 0);
    lord_reset_keyboard();
}

#endif




/*
  shows confirm scroll
 */


void
gui_confirm_scroll(char *text)
{
    const char *scroll_text[7];
    char *tmptext;
    const char *formatted_text[MAX_MESSAGELINES];
    int lines, i;

    tmptext = format_text(text, &lines, DEFAULT_SCROLL_WIDTH, formatted_text);

    for (i = 0; i < lines && formatted_text[i][0] && i < 6; ++i)
        scroll_text[i] = formatted_text[i];

    scroll_text[i++] = "Are you sure? (Y/N)";

    for (; i < 7; ++i)
        scroll_text[i] = "";

    draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, 7, scroll_text);
    free(tmptext);
}


/*
  show options menu
 */


void
dialog_options_show(void)
{

    const char *text[7] = {
        "OPTIONS",
        "L. Load Game",
        "S. Save Game",
        "Q. Quit Game",
        "P. Pause Game",
        "M. Start/Stop Music",
        "X. Exit"
    };

    dialog_mode = DIALOG_OPTIONS;
    dialog_confirm = 0;

    draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, 7, text);

}



/*
  show save menu
 */


void
dialog_save_show(void)
{

    const char *text[7] = {
        "Save Game",
        "1. Game One",
        "2. Game Two",
#if PIXEL_PRECISE
        "X. Exit",
        "",
        "",
        ""
#else
        "3. Game Three",
        "4. Game Four",
        "5. Game Five",
        "X. Exit"
#endif
    };

    dialog_mode = DIALOG_SAVE;
    dialog_confirm = 0;

    draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, 7, text);

}



/*
  show load menu
 */


void
dialog_load_show(void)
{

    const char *text[7] = {
        "Load Game",
        "1. Game One",
        "2. Game Two",
#if PIXEL_PRECISE
        "X. Exit",
        "",
        "",
        ""
#else
        "3. Game Three",
        "4. Game Four",
        "5. Game Five",
        "X. Exit"
#endif
    };

    dialog_mode = DIALOG_LOAD;
    dialog_confirm = 0;

    draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, 7, text);

}




/*
  show options message
 */


void
dialog_options_message_show(const char *text[7])
{
    dialog_mode = DIALOG_MESSAGE;
    draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, 7, text);
}






/*
  parse options menu keyboard input
*/



void
dialog_options_key(int key)
{

    const char *paused_text[7] = { "Paused...", "", "", "", "", "", "" };


    if (dialog_confirm) {
        if (key == 'y')
            exit(0);
        if (key == 'n' || key == 'x')
            quit_menu();
    }

    switch (key) {
    case 'x':
        quit_menu();
        break;

    case 'p':
        dialog_options_message_show(paused_text);
        break;

    case 's':
        dialog_save_show();
        break;

    case 'l':
        dialog_load_show();
        break;

    case 'm':
        toggle_music();
        quit_menu();
        break;

    case 'q':
        gui_confirm_scroll("Quit Game");
        dialog_confirm = 1;

        break;

    default:
        break;

    }

}



/*
  parse save/load menu keyboard input
*/



void
dialog_saveload_key(int key)
{
    int n;

    const char *saved_text[7] =
        { "Save current game,", "done.", "", "", "", "", "" };
    const char *loaded_text[7] =
        { "Load previous game,", "done.", "", "", "", "", "" };

    if (key == 'x')
        quit_menu();

    if (key >= '1' && key <= '5') {
        n = key - '0';

#if PIXEL_PRECISE
        if (n > 2)
            return;
#endif
        if (dialog_mode == DIALOG_SAVE) {
            if (game_save(n))
                dialog_options_message_show(saved_text);
            return;
        }
        if (dialog_mode == DIALOG_LOAD) {
            if (game_load(n)) {
                quit_menu();
                dialog_options_message_show(loaded_text);
            }
            return;
        }
    }
}




/*
  show view dialog
 */


void
dialog_view_show(void)
{
    const char *text[7];
    char lines[7][25];

    int i;

    main_menu_show();
    dialog_mode = DIALOG_VIEW;

    sprintf(lines[0], "                    ");
    strncpy(lines[0] + (20 - strlen(choosed_character->name)) / 2,
            choosed_character->name, 20);


    sprintf(lines[1], " Silver:%d", game_get_silver());
    lines[2][0] = 0;
    sprintf(lines[3], " Dex.:%-4d Str.:%-4d",
            choosed_character->dex, choosed_character->str);
    sprintf(lines[4], " End.:%-4d Luck:%-4d",
            choosed_character->end, choosed_character->luck);
    sprintf(lines[5], " Life:%-4d Will:%-4d",
            choosed_character->life, choosed_character->will);
    sprintf(lines[6], "       eXit");

    for (i = 0; i < 7; ++i)
        text[i] = lines[i];

    draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, 7, text);

}




/*
  parse view keyboard input
*/


void
dialog_view_key(int key)
{
    if (key == KEY_ENTER || key == ' ' || key == 'x')
        main_menu_show();
}




/*
  draw list dialog
 */

int dialog_list_offset;
int dialog_list_num;
int dialog_list_codes[10];
char *dialog_list_name;
char *dialog_list_names[10];
int dialog_list_can_scroll;

void
dialog_list_draw(void)
{
    const char *text[12];
    char lines[12][25];

    int i;
    int num_lines;

    int index;
    int object;

    char status_char;

    num_lines = 5;

    if (!PIXEL_PRECISE && dialog_list_num > 5)
        num_lines = dialog_list_num;

    strncpy(lines[0], dialog_list_name, 20);

    for (i = 0; i < num_lines; ++i)
        if (dialog_list_offset + i < dialog_list_num) {
            index = dialog_list_offset + i;
            status_char = ' ';
            if (dialog_mode == DIALOG_USE || dialog_mode == DIALOG_DISCARD
                || dialog_mode == DIALOG_TRADE) {
                object = choosed_character->items[index];

                if (choosed_character->item_used[index]) {
                    if (object_is_ring(object)
                        && choosed_character->ring_mode == 2)
                        status_char = '!';
                    if (object_is_armour(object))
                        status_char = '&';
                    if (object_is_shield(object))
                        status_char = '^';
                    if (object_is_weapon(object))
                        status_char = '*';
                }
            }
            sprintf(lines[i + 1], "%d.%c%s", (i == 9) ? 0 : i + 1,
                    status_char, dialog_list_names[index]);
        }
        else
            strncpy(lines[i + 1], "", 20);

    dialog_list_can_scroll = dialog_list_num > num_lines;

    if (dialog_list_can_scroll)
        strncpy(lines[num_lines + 1], "   UP DOWN eXit", 20);
    else
        strncpy(lines[num_lines + 1], "       eXit", 20);

    for (i = 0; i < num_lines + 2; ++i)
        text[i] = lines[i];

    draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, num_lines + 2, text);

}





/*
  show attack dialog
 */



void
dialog_attack_show(void)
{
    int i;

    main_menu_show();
    dialog_mode = DIALOG_ATTACK;

    dialog_list_num = 0;

    if (combat_get_mode())
        dialog_list_num =
            combat_in_bow_range(choosed_character, dialog_list_codes);

    if (dialog_list_num == 0)
        dialog_list_num =
            map_npc_stay_near(choosed_character, dialog_list_codes, 0);

    if (dialog_list_num <= 0) {
        main_menu_show();
        return;
    }

    for (i = 0; i < dialog_list_num; ++i)
        dialog_list_names[i] = character_get(dialog_list_codes[i])->name;


    dialog_list_name = "ATTACK";
    dialog_list_offset = 0;

    dialog_list_draw();

}






/*
  show skills dialog
 */


void
dialog_skill_show(void)
{
    int i;

    main_menu_show();
    dialog_mode = DIALOG_SKILL;

    dialog_list_num = choosed_character->skills_num;

    for (i = 0; i < dialog_list_num; ++i) {
        dialog_list_codes[i] = choosed_character->skills[i];
        dialog_list_names[i] = object_name(dialog_list_codes[i]);
    }

    dialog_list_name = "SKILL";
    dialog_list_offset = 0;

    dialog_list_draw();

}





/*
  show magic dialog
 */


void
dialog_magic_show(void)
{
    int i;

    main_menu_show();
    dialog_mode = DIALOG_MAGIC;

    dialog_list_num = choosed_character->spells_num;

    for (i = 0; i < dialog_list_num; ++i) {
        dialog_list_codes[i] = i;
        dialog_list_names[i] = object_name(choosed_character->spells[i]);
    }

    dialog_list_name = "MAGIC";
    dialog_list_offset = 0;

    dialog_list_draw();

}



/*
  show use dialog
 */


void
dialog_use_main_show(void)
{

    main_menu_show();
    dialog_mode = DIALOG_USE_MAIN;

    dialog_list_num = 3;

    dialog_list_codes[0] = 1;
    dialog_list_names[0] = "Discard";
    dialog_list_codes[1] = 2;
    dialog_list_names[1] = "Equip/Use";
    dialog_list_codes[2] = 3;
    dialog_list_names[2] = "Trade";

    dialog_list_name = "USE";
    dialog_list_offset = 0;

    dialog_list_draw();

}



/*
  show discard dialog
 */


void
dialog_discard_show(void)
{
    int i;

    main_menu_show();
    dialog_mode = DIALOG_DISCARD;

    dialog_list_num = choosed_character->items_num;

    for (i = 0; i < dialog_list_num; ++i) {
        dialog_list_codes[i] = i;
        dialog_list_names[i] = object_name(choosed_character->items[i]);
    }

    dialog_list_name = "DISCARD";
    dialog_list_offset = 0;

    dialog_list_draw();

}




/*
  show discard confirm dialog
 */

int dialog_what_discard;

void
dialog_discard_confirm_show(int what)
{

    char text[25];

    if (object_is_ring(choosed_character->items[what])) {
        dialog_discard_show();
        return;
    }

    main_menu_show();
    dialog_mode = DIALOG_DISCARD_CONFIRM;

    dialog_what_discard = what;

    sprintf(text, "Discard %s?", object_name(choosed_character->items[what]));
    gui_confirm_scroll(text);

}



/*
  parse discard confirm keyboard input
*/

void
dialog_discard_confirm_key(int key)
{
    int what;

    if (key == 'y') {
        what = choosed_character->items[dialog_what_discard];

        character_discard_item(choosed_character, dialog_what_discard);

        if (what == OBJECT_TORCH)
            game_check_light();

        dialog_discard_show();
    }
    if (key == 'x' || key == 'n')
        dialog_discard_show();
}





/*
  show use dialog
 */


void
dialog_use_show(void)
{
    int i;

    main_menu_show();
    dialog_mode = DIALOG_USE;

    dialog_list_num = choosed_character->items_num;

    for (i = 0; i < dialog_list_num; ++i) {
        dialog_list_codes[i] = i;
        dialog_list_names[i] = object_name(choosed_character->items[i]);
    }

    dialog_list_name = "EQUIP/USE";
    dialog_list_offset = 0;

    dialog_list_draw();

}



/*
  show get dialog
 */


Uint8 *dialog_get_objects;
int dialog_get_to_buy;
char dialog_buy_texts[11][20];


void
dialog_get_show(void)
{
    int i;
    CommandSpot *spot;

    main_menu_show();

    if (!(spot = game_get_actual_spot()))
        return;

    if (!(dialog_get_objects = spot_get_objects(spot, &dialog_get_to_buy)))
        return;

    dialog_mode = DIALOG_GET;

    for (i = 0; dialog_get_objects[i] != 0xff && i < 10; ++i) {
        dialog_list_codes[i] = i;
        if (dialog_get_to_buy) {
            sprintf(dialog_buy_texts[i],
                    "%-14s%d", object_name(dialog_get_objects[i]),
                    object_price(dialog_get_objects[i]));
            dialog_list_names[i] = dialog_buy_texts[i];
        }
        else
            dialog_list_names[i] = object_name(dialog_get_objects[i]);
    }

    dialog_list_num = i;

    if (dialog_get_to_buy) {
        sprintf(dialog_buy_texts[10], "Silver Pennies:%d", game_get_silver());
        dialog_list_name = dialog_buy_texts[10];
    }
    else
        dialog_list_name = "TAKE ITEM(S)";
    dialog_list_offset = 0;

    dialog_list_draw();

}




/*
  show trade to who dialog
 */


void
dialog_trade_to_show(void)
{

    int i, j;

    if (choosed_character->items_num == 0) {
        dialog_use_main_show();
        return;
    }

    if (choosed_character->life > 0) {
        dialog_list_num =
            map_npc_stay_near(choosed_character, dialog_list_codes, 1);
    }
    else {
        /* trading from dead character */
        dialog_list_num = game_get_party(dialog_list_codes);

        if (dialog_list_num <= 1) {
            cartoon_play("cart10");
            exit(0);
        }

        /* do not show dead characters */

        for (i = 0, j = 0; i < dialog_list_num; ++i)
            if (character_get(dialog_list_codes[i])->life > 0)
                dialog_list_codes[j++] = dialog_list_codes[i];

        dialog_list_num = j;

    }

    if (dialog_list_num <= 0) {
        dialog_use_main_show();
        return;
    }

    for (i = 0; i < dialog_list_num; ++i)
        dialog_list_names[i] = character_get(dialog_list_codes[i])->name;

    main_menu_show();
    dialog_mode = DIALOG_TRADE_TO;

    dialog_list_name = "TRADE TO?";
    dialog_list_offset = 0;

    dialog_list_draw();

}



/*
  show trade dialog
 */

int dialog_trade_to_who;

void
dialog_trade_show(int to_who)
{
    int i;

    main_menu_show();
    dialog_mode = DIALOG_TRADE;
    dialog_trade_to_who = to_who;

    dialog_list_num = choosed_character->items_num;

    for (i = 0; i < dialog_list_num; ++i) {
        dialog_list_codes[i] = i;
        dialog_list_names[i] = object_name(choosed_character->items[i]);
    }

    dialog_list_name = "TRADE";
    dialog_list_offset = 0;

    dialog_list_draw();

}



/*
  show trade confirm dialog
 */

int dialog_what_trade;

void
dialog_trade_confirm_show(int what)
{

    char text[25];

    main_menu_show();
    dialog_mode = DIALOG_TRADE_CONFIRM;

    dialog_what_trade = what;

    sprintf(text, "Trade %s to %s?",
            object_name(choosed_character->items[what]),
            character_get(dialog_trade_to_who)->name);

    gui_confirm_scroll(text);

}



/*
  parse trade confirm keyboard input
*/

void
dialog_trade_confirm_key(int key)
{
    Character *character;
    int what;

    if (key == 'y') {
        character = character_get(dialog_trade_to_who);

        what = choosed_character->items[dialog_what_trade];
        character_add_item(character, what);
        character_discard_item(choosed_character, dialog_what_trade);

        if (what == OBJECT_TORCH)
            game_check_light();

        if (spot_action(game_get_actual_spot(), SPOT_ACTION_TRADE,
                        what, dialog_trade_to_who))
            return;

        dialog_trade_show(dialog_trade_to_who);

    }
    if (key == 'x' || key == 'n')
        dialog_trade_show(dialog_trade_to_who);
}






/*
  show talk dialog
 */


void
dialog_talk_main_show(void)
{

    main_menu_show();
    dialog_mode = DIALOG_TALK_MAIN;

    dialog_list_num = 3;

    dialog_list_codes[0] = 1;
    dialog_list_names[0] = "Recruit";
    dialog_list_codes[1] = 2;
    dialog_list_names[1] = "Dismiss";
    dialog_list_codes[2] = 3;
    dialog_list_names[2] = "Talk";

    dialog_list_name = "TALK";
    dialog_list_offset = 0;

    dialog_list_draw();

}



/*
  show recruit dialog
 */


void
dialog_recruit_show(void)
{

    int i;

    dialog_list_num =
        map_npc_stay_near(choosed_character, dialog_list_codes, 0);

    if (dialog_list_num <= 0) {
        dialog_talk_main_show();
        return;
    }

    for (i = 0; i < dialog_list_num; ++i)
        dialog_list_names[i] = character_get(dialog_list_codes[i])->name;

    main_menu_show();
    dialog_mode = DIALOG_RECRUIT;

    dialog_list_name = "RECRUIT WHO?";
    dialog_list_offset = 0;

    dialog_list_draw();

}




/*
  show dismiss dialog
 */


void
dialog_dismiss_show(void)
{

    int i;
    Character *leader;
    Character *character;
    int n;
    int codes[10];

    leader = game_get_leader();
    n = game_get_party(codes);

    for (i = 0, dialog_list_num = 0; i < n; ++i) {
        character = character_get(codes[i]);
        if (character != leader && character != choosed_character) {
            dialog_list_names[dialog_list_num] = character->name;
            dialog_list_codes[dialog_list_num] = character->id;
            dialog_list_num++;
        }
    }

    if (dialog_list_num <= 0) {
        dialog_talk_main_show();
        return;
    }


    main_menu_show();
    dialog_mode = DIALOG_DISMISS;

    dialog_list_name = "DISMISS WHO?";
    dialog_list_offset = 0;

    dialog_list_draw();

}



/*
  show talk to who dialog
 */


void
dialog_talk_who_show(void)
{

    int i;

    dialog_list_num =
        map_npc_stay_near(choosed_character, dialog_list_codes, 0);

    if (dialog_list_num <= 0) {
        dialog_talk_main_show();
        return;
    }

    for (i = 0; i < dialog_list_num; ++i)
        dialog_list_names[i] = character_get(dialog_list_codes[i])->name;

    main_menu_show();
    dialog_mode = DIALOG_TALK_WHO;

    dialog_list_name = "TALK TO WHO?";
    dialog_list_offset = 0;

    dialog_list_draw();

}



/*
  show talk dialog
 */


Character *dialog_questioned_character;
int dialog_talk_character_text;
int dialog_talk_character_id;

void
dialog_talk_show(int character_id, int text_id)
{

    Character *character;
    Character *tmpcharacter;
    char *tmptext;
    const char *formatted_text[MAX_MESSAGELINES];
    const char *scroll_text[7];
    char *text;
    int lines;
    int i;

    /* character_id==-1 for wizard mode */

    dialog_talk_character_text = text_id;
    dialog_talk_character_id = character_id;

    if (character_id != -1) {
        wizard_mode = 0;

        character = character_get(character_id);

        if (character->texts[0] == 0 && character->texts[1] == 0) {
            /* character has nothing to say */
            dialog_talk_who_show();
            return;
        }

        if (text_id >= 0x100) {
            text = "I don't know any more than you do about that.";
        }
        else {
            if (text_id < 0)
                text_id = (Uint8)character->texts[0];
#ifdef EXTENDED
            if (character->pythontexts)
                text = pythonspot_get_text(text_id);
            else
#endif
                text = game_get_text(text_id);
        }
    }                           /* character_id!=-1 */
    else {
#ifdef WIZARD_MODE
        character = character_get(0xa5);        /* Gandalf */
        if (text_id < 0)
            text = "Wizard mode. Cast a spell.";
        else
            text = wizard_text;
        wizard_mode = 1;
#else
        character = character_get(character_id);        /* error */
        text = "";
#endif
    }

    tmpcharacter = choosed_character;
    choosed_character = character;
    main_menu_show();
    choosed_character = tmpcharacter;
    dialog_mode = DIALOG_TALK;

    tmptext = format_text(text, &lines, DEFAULT_SCROLL_WIDTH, formatted_text);

    if (lines > 6)
        lines = 6;

    for (i = 0; i < 6; ++i) {
        if (i < lines)
            scroll_text[i] = formatted_text[i];
        else
            scroll_text[i] = "";
    }

    if (!wizard_mode &&
        ((Uint8)character->texts[1] == 0xff ||
         (dialog_talk_character_text >= 0
          && dialog_talk_character_text < 0x100
          && spot_has_action(game_get_actual_spot(), SPOT_ACTION_QUESTION,
                             dialog_talk_character_text,
                             dialog_talk_character_id))
        )) {
        scroll_text[6] = "       eXit";
        dialog_questioned_character = NULL;
    }
    else {
        dialog_questioned_character = character;
        scroll_text[6] = "   Question eXit";
    }

    draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, 7, scroll_text);

    free(tmptext);

}



/*
  show question dialog
 */

char dialog_question_text[DEFAULT_SCROLL_WIDTH + 1];
int dialog_question_len;
const char *dialog_question_scroll_text[7];
int dialog_cursor;

void
dialog_question_show()
{

    Character *tmpcharacter;

    tmpcharacter = choosed_character;
    choosed_character = dialog_questioned_character;
    main_menu_show();
    choosed_character = tmpcharacter;
    dialog_mode = DIALOG_QUESTION;

    dialog_question_len = 0;
    dialog_question_text[0] = 0;
    dialog_cursor = 0;

    dialog_question_scroll_text[0] = "ABOUT WHAT?";
    dialog_question_scroll_text[1] = dialog_question_text;
    dialog_question_scroll_text[2] = "";
    dialog_question_scroll_text[3] = "";
    dialog_question_scroll_text[4] = "";
    dialog_question_scroll_text[5] = "";
    dialog_question_scroll_text[6] = "";

    draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, 7, dialog_question_scroll_text);
}



/*
  parse question keyboard input
*/


void
dialog_question_key(int key)
{

    char *texts;
    int answer, ans;
    int i, j, len;
#ifdef WIZARD_MODE
    int reg;
#endif

    if (key == KEY_BACKSPACE)
        dialog_question_len--;

    if (key == KEY_ENTER) {

#ifdef WIZARD_MODE
        if (!wizard_mode) {
#endif
            if (dialog_question_len == 0) {
                dialog_talk_who_show();
                return;
            }

            dialog_question_text[dialog_question_len] = 0;
            answer = 0x100;
            texts = dialog_questioned_character->texts + 1;
            i = 0;
            while (spot_question_letter(texts[i]) || texts[i] == 0) {

                len = 0;
                while (spot_question_letter(texts[i + len]) && len < 20)
                    ++len;

                if (len == 20) {
                    j = i + len;
                    --len;
                }
                else {
                    if (texts[i + len] == 0)
                        j = i + len + 2;
                    else
                        j = i + len + 1;
                }

                ans = (Uint8)texts[j - 1];

                if (len == 0)
                    answer = ans;

                if (!strncmp(texts + i, dialog_question_text, len)) {
                    answer = ans;
                    break;
                }

                i = j;
            }
            dialog_talk_show(dialog_questioned_character->id, answer);
#ifdef WIZARD_MODE
        }
        else {
            int spell[10];
            char spell_type = 0;
            int spell_size;

            if (dialog_question_len == 0) {
                quit_menu();
                return;
            }

            spell_size = 0;
            spell_type = dialog_question_text[0];

            i = 1;
            while (i < dialog_question_len) {
                while (dialog_question_text[i] == ' '
                       && i < dialog_question_len)
                    ++i;
                if (i < dialog_question_len) {
                    spell[spell_size] = 0;
                    while (dialog_question_text[i] != ' '
                           && i < dialog_question_len) {
                        if (dialog_question_text[i] >= '0'
                            && dialog_question_text[i] <= '9')
                            spell[spell_size] =
                                0x10 * spell[spell_size] +
                                dialog_question_text[i] - '0';
                        else if (dialog_question_text[i] >= 'A'
                                 && dialog_question_text[i] <= 'F')
                            spell[spell_size] =
                                0x10 * spell[spell_size] +
                                dialog_question_text[i] - 'A' + 10;
                        else
                            goto spell_foul;

                        ++i;
                    }
                    spell_size++;
                }
            }

            sprintf(wizard_text, "Spell casted.");


            switch (spell_type) {

#ifdef TTT
            case 'B':
                map_enter_building(spell[0]);
                game_leader_teleport(0, 16 * 4 * 4, 16 * 4 * 4, 0xff, 0xff);
                break;
#endif

            case 'E':
                reg = !game_get_register(spell[0]);
                sprintf(wizard_text, "reg: %d", reg);
                game_set_register(spell[0], reg);
                break;

            case 'H':
                sprintf(wizard_text,
                        "Building, Help, Location, Teleport, Map, Day, Night, Spots, Get, Recruit, Kill, rEg, Paragraph");
                break;

            case 'L':
                sprintf(wizard_text, "loc: %x %x",
                        game_get_leader()->x, game_get_leader()->y);
                break;

            case 'M':
                quit_menu();
                game_leader_teleport(0, 0xffff, 0xffff, 0xff, spell[0]);
                return;

            case 'T':
                game_leader_teleport(0, spell[0], spell[1], 0xff, 0xff);
                break;

            case 'D':
                map_set_frame(0x400);
                break;

            case 'N':
                map_set_frame(0x0);
                break;

            case 'S':
                gui_show_spots = !gui_show_spots;
                break;


            case 'P':
                /* read hex number as dec */
                j = spell[0] % 16;
                spell[0] /= 16;
                j += (spell[0] % 16) * 10;
                spell[0] /= 16;
                j += (spell[0] % 16) * 100;
                quit_menu();
                gui_paragraph(j);
                return;

            case 'R':
                map_character_teleport(character_get(spell[0]), 0,
                                       game_get_leader()->x,
                                       game_get_leader()->y, 0xff,
                                       game_get_leader()->map);
                game_recruit(character_get(spell[0]), 1);
                break;

            case 'G':
                if (spell[0] == OBJECT_SILVER)
                    game_add_silver(1000);
                if (object_is_item(spell[0]))
                    character_add_item(game_get_leader(), spell[0]);
                if (object_is_spell(spell[0]))
                    character_add_spell(game_get_leader(), spell[0]);
                if (object_is_skill(spell[0]))
                    character_add_skill(game_get_leader(), spell[0]);
                break;


            case 'K':
                if (combat_get_mode())
                    combat_genocide();
                else
                    spell_type = 0;
                break;

            default:
                spell_type = 0;
            }


            if (!spell_type) {
              spell_foul:
                sprintf(wizard_text,
                        "Thou art not a true wizard. Thy foul spell failed\n");
            }

            dialog_talk_show(-1, 0);
        }
#endif

    }


    if (dialog_question_len >= DEFAULT_SCROLL_WIDTH - 1)
        return;

    if (key >= 'a' && key <= 'z')
        dialog_question_text[dialog_question_len++] = key - 'a' + 'A';

    if (key >= '0' && key <= '9')
        dialog_question_text[dialog_question_len++] = key;

    if (key == ' ' || key == '\'' || key == '*' || key == '-')
        dialog_question_text[dialog_question_len++] = key;



}


/*
  parse talk keyboard input
*/


void
dialog_talk_key(int key)
{

    if (key == 'x') {
#ifdef WIZARD_MODE
        if (wizard_mode) {
            quit_menu();
            return;
        }
#endif
        if (dialog_talk_character_text >= 0
            && dialog_talk_character_text < 0x100
            && spot_action(game_get_actual_spot(), SPOT_ACTION_QUESTION,
                           dialog_talk_character_text,
                           dialog_talk_character_id))
            return;

        dialog_talk_who_show();
    }
    if (key == 'q')
        dialog_question_show();
}




/*
  show choose dialog
 */


void
dialog_choose_show(void)
{

    int i;

    dialog_list_num = game_get_party(dialog_list_codes);

    if (dialog_list_num <= 0) {
        dialog_talk_main_show();
        return;
    }

    for (i = 0; i < dialog_list_num; ++i)
        dialog_list_names[i] = character_get(dialog_list_codes[i])->name;

    main_menu_show();
    dialog_mode = DIALOG_CHOOSE;

    dialog_list_name = "CHOOSE WHO";
    dialog_list_offset = 0;

    dialog_list_draw();

}




/*
  show leader dialog
 */


void
dialog_leader_show(void)
{

    int i;

    dialog_list_num = game_get_party(dialog_list_codes);

    if (dialog_list_num <= 0) {
        dialog_talk_main_show();
        return;
    }

    for (i = 0; i < dialog_list_num; ++i)
        dialog_list_names[i] = character_get(dialog_list_codes[i])->name;

    main_menu_show();
    dialog_mode = DIALOG_LEADER;

    dialog_list_name = "SELECT_LEADER";
    dialog_list_offset = 0;

    dialog_list_draw();

}




/*
  parse list keyboard input
*/


void
dialog_list_key(int key)
{

    int i, item, code;
    int spell_code;
    Character *character;


    if (dialog_list_can_scroll && (key == 'd' || key == 'u')) {
        if (key == 'd' && dialog_list_offset + 5 < dialog_list_num) {
            dialog_list_offset++;
            dialog_list_draw();
        }

        if (key == 'u' && dialog_list_offset > 0) {
            dialog_list_offset--;
            dialog_list_draw();
        }

    }

    if (key == 'x') {
        switch (dialog_mode) {
        case DIALOG_DISCARD:
        case DIALOG_USE:
        case DIALOG_TRADE_TO:
            dialog_use_main_show();
            break;

        case DIALOG_TRADE:
            dialog_trade_to_show();
            break;

        case DIALOG_TALK_WHO:
        case DIALOG_RECRUIT:
        case DIALOG_DISMISS:
            dialog_talk_main_show();
            break;

        default:
            main_menu_show();
        }
    }

    if (key < '0' || key > '9')
        return;

    i = key - '0';
    if (i == 0)
        i = 10;

    if (dialog_list_can_scroll && i > 5)
        return;

    i += dialog_list_offset - 1;

    if (i >= dialog_list_num)
        return;

    code = dialog_list_codes[i];


    switch (dialog_mode) {

    case DIALOG_ATTACK:
        if (combat_get_mode()) {
            main_menu_show();
            combat_attack(choosed_character, combat_get_enemy(code));
        }
        else {
            if (spot_action
                (game_get_actual_spot(), SPOT_ACTION_ATTACK, 0xff, code))
                return;
        }
        break;

    case DIALOG_SKILL:
        if (code == 0x48)       /*climb */
            map_set_climb_mode(1);
        else
            map_set_climb_mode(0);
        if (spot_action
            (game_get_actual_spot(), SPOT_ACTION_SKILL, code, 0xff))
            quit_menu();
        else
            dialog_skill_show();
        return;

    case DIALOG_MAGIC:
        if (choosed_character->life <= 5)
            break;
        if (dialog_list_names[code][0] != '!')
            choosed_character->life -= lord_rnd(4);

        spell_code = choosed_character->spells[code];
        if (!combat_get_mode()) {
            if (spot_has_action(game_get_actual_spot(), SPOT_ACTION_MAGIC,
                                spell_code, 0xff)) {
                if (dialog_list_names[code][0] == '!') {
                    for (i = code; i + 1 < choosed_character->spells_num; ++i)
                        choosed_character->spells[i] =
                            choosed_character->spells[i + 1];
                    choosed_character->spells_num--;
                }
                music_spell_performed();
                spot_action(game_get_actual_spot(), SPOT_ACTION_MAGIC,
                            spell_code, 0xff);
            }
            else {
                quit_menu();
                if (spell_code == SPELL_ILLUMINATE) {
                    music_spell_performed();
                    map_set_light(2);
                    return;
                }
                music_spell_failed();
            }
            return;
        }                       /* combat_get_mode */
        else {
            if (spell_code == SPELL_ILLUMINATE) {
                music_spell_performed();
                map_set_light(2);
                return;
            }
        }
        main_menu_show();
        break;

    case DIALOG_USE_MAIN:
        if (code == 1)
            dialog_discard_show();
        if (code == 2)
            dialog_use_show();
        if (code == 3)
            dialog_trade_to_show();
        break;

    case DIALOG_DISCARD:
        dialog_discard_confirm_show(code);
        break;

    case DIALOG_USE:
        if (!spot_action
            (game_get_actual_spot(), SPOT_ACTION_OBJECT,
             choosed_character->items[code], 0xff)) {
            character_use(choosed_character, code);
            dialog_use_show();
        }
        break;

    case DIALOG_GET:
        item = dialog_get_objects[code];

        if (dialog_get_to_buy
            && spot_has_action(game_get_actual_spot(), SPOT_ACTION_BUY,
                               item, 0xff)) {
            if (game_pay_silver(object_price(item))
                && spot_action(game_get_actual_spot(), SPOT_ACTION_BUY,
                               item, 0xff))
                return;
        }

        if (dialog_get_to_buy &&
            (choosed_character->items_num == 10
             || !game_pay_silver(object_price(item))))
            break;

        if (!dialog_get_objects[code] == 0 &&
            !character_add_item(choosed_character, item) &&
            !character_add_skill(choosed_character, item))
            break;

        if (!dialog_get_to_buy) {
            for (; dialog_get_objects[code] != 0xff && code + 1 < 10; ++code)
                dialog_get_objects[code] = dialog_get_objects[code + 1];
            dialog_get_objects[code] = 0xff;

            if (item == 0) {    /* silver pennies */
                for (; dialog_get_objects[code] == 0xff && code < 12; ++code);
                if (dialog_get_objects[code] != 0xff)
                    game_add_silver(dialog_get_objects[code]);

            }

            if (spot_action
                (game_get_actual_spot(), SPOT_ACTION_GET, item, 0xff))
                return;
            else
                dialog_get_show();
        }                       /* if( !dialog_get_to_buy ) */
        else {
            dialog_get_show();
        }

        break;

    case DIALOG_TRADE_TO:
        dialog_trade_show(code);
        break;

    case DIALOG_TRADE:
        character = character_get(dialog_trade_to_who);
        if (game_in_party(character)) {

            item = choosed_character->items[code];

            if (object_is_ring(item) && choosed_character->life > 0)
                break;

            if (character_add_item(character, item)) {
                character_discard_item(choosed_character, code);

                if (item == OBJECT_TORCH)
                    game_check_light();
            }

            if (object_is_ring(item)) {
                character->ring_mode = 1;
                choosed_character->ring_mode = 0;
            }

            dialog_trade_show(dialog_trade_to_who);
        }
        else {
            if (spot_has_action(game_get_actual_spot(), SPOT_ACTION_TRADE,
                                choosed_character->items[code],
                                dialog_trade_to_who))
                dialog_trade_confirm_show(code);
            else
                dialog_trade_show(dialog_trade_to_who);
        }
        break;

    case DIALOG_TALK_MAIN:
        if (code == 1)
            dialog_recruit_show();
        if (code == 2)
            dialog_dismiss_show();
        if (code == 3)
            dialog_talk_who_show();
        break;

    case DIALOG_RECRUIT:
        character = character_get(code);
        game_recruit(character, 0);
        dialog_recruit_show();
        break;

    case DIALOG_DISMISS:
        character = character_get(code);
        if (!character->ring_mode)
            game_dismiss(character);
        dialog_dismiss_show();
        break;

    case DIALOG_TALK_WHO:
        if (spot_action(game_get_actual_spot(), SPOT_ACTION_TALK, 0xff, code))
            break;
        dialog_talk_show(code, -1);
        break;

    case DIALOG_CHOOSE:
        choosed_character = character_get(code);
        main_menu_show();
        break;

    case DIALOG_LEADER:
        character = character_get(code);
        if (game_set_leader(character))
            choosed_character = character;
        main_menu_show();
        break;

    default:
        break;
    }

}



/*
  parse main menu keyboard input
*/
void
main_menu_key(int key)
{


    switch (key) {
    case 'x':
    case ' ':
        if (combat_get_mode()) {
            choosed_character->ap = 0;
            combat_character_finished();
        }
        else
            quit_menu();
        break;

    case 'a':
        dialog_attack_show();
        break;

    case 'c':
        if (!combat_get_mode())
            dialog_choose_show();
        break;

    case 'l':
        if (!combat_get_mode())
            dialog_leader_show();
        break;

    case 'm':
        dialog_magic_show();
        break;

#if !PIXEL_PRECISE
    case 'h':
        quit_menu();
        gui_help_show();
        break;
#endif

    case 'q':
        gui_confirm_scroll("Quit Game");
        dialog_mode = DIALOG_OPTIONS;
        dialog_confirm = 1;
        break;

    case 's':
        dialog_skill_show();
        break;

    case 't':
        if (!combat_get_mode())
            dialog_talk_main_show();
        break;

    case 'u':
        dialog_use_main_show();
        break;

    case 'g':
        if (!combat_get_mode())
            dialog_get_show();
        break;

    case 'v':
        dialog_view_show();
        break;

#ifdef WIZARD_MODE
    case 'w':
        if (lord_key_shift())
            dialog_print_active_spot();
        else
            dialog_talk_show(-1, -1);
        break;
#endif

    default:
        break;

    }

}








/*
  gui frame - returns nonzero if a dialog is active (game paused)
*/

int
gui_frame(void)
{

    int key;

    if (dialog_mode == DIALOG_QUESTION) {
        dialog_question_text[dialog_question_len] =
            0x80 - 4 + abs(dialog_cursor);
        dialog_question_text[dialog_question_len + 1] = 0;

        draw_scroll(0, 0, -DEFAULT_SCROLL_WIDTH, 7,
                    dialog_question_scroll_text);
        dialog_cursor++;
        if (dialog_cursor == 4)
            dialog_cursor = -2;
    }


    key = lord_get_key();

    if (key == 0 && !lord_key_esc())
        return dialog_mode;

    if (dialog_mode == DIALOG_NONE) {

        if (lord_key_esc()) {
#if DEMO
            exit(0);
#endif
            lord_reset_keyboard();
            dialog_options_show();
        }

        switch (key) {
        case ' ':
            main_menu_show();
            break;

        case 'a':
            dialog_attack_show();
            break;

        case 'c':
            dialog_choose_show();
            break;

        case 'l':
            dialog_leader_show();
            break;

        case 'm':
            dialog_magic_show();
            break;

#if !PIXEL_PRECISE
        case 'h':
            gui_help_show();
            break;
#endif

        case 'o':
#if !DEMO
            dialog_options_show();
#endif
            break;

        case 'q':
            gui_confirm_scroll("Quit Game");
            dialog_mode = DIALOG_OPTIONS;
            dialog_confirm = 1;
            break;

        case 's':
            dialog_skill_show();
            break;

        case 't':
            dialog_talk_main_show();
            break;

        case 'u':
            dialog_use_main_show();
            break;

        case 'g':
            dialog_get_show();
            break;

        case 'v':
            dialog_view_show();
            break;

#ifdef WIZARD_MODE
        case 'w':
            if (lord_key_shift())
                dialog_print_active_spot();
            else
                dialog_talk_show(-1, -1);
            break;
#endif

        default:
            break;
        }
    }
    else {

        if (lord_key_esc()) {
#if DEMO
            exit(0);
#endif
            lord_reset_keyboard();
            quit_menu();
        }

        switch (dialog_mode) {
        case MAIN_MENU:
            main_menu_key(key);
            break;

        case DIALOG_OPTIONS:
            dialog_options_key(key);
            break;

        case DIALOG_SAVE:
        case DIALOG_LOAD:
            dialog_saveload_key(key);
            break;

        case DIALOG_MESSAGE:
        case DIALOG_BOOK:
            dialog_message_key(key);
            break;

        case DIALOG_PARAGRAPH:
            dialog_paragraph_key(key);
            break;

        case DIALOG_SPOT_PRINT:
            dialog_print_active_spot_key(key);
            break;

        case DIALOG_MESSAGE_YN:
            dialog_message_yn_key(key);
            break;

        case DIALOG_VIEW:
            dialog_view_key(key);
            break;

        case DIALOG_SKILL:
        case DIALOG_MAGIC:
        case DIALOG_USE_MAIN:
        case DIALOG_DISCARD:
        case DIALOG_USE:
        case DIALOG_GET:
        case DIALOG_TRADE_TO:
        case DIALOG_TRADE:
        case DIALOG_TALK_MAIN:
        case DIALOG_TALK_WHO:
        case DIALOG_RECRUIT:
        case DIALOG_DISMISS:
        case DIALOG_LEADER:
        case DIALOG_CHOOSE:
        case DIALOG_ATTACK:
            dialog_list_key(key);
            break;

        case DIALOG_DISCARD_CONFIRM:
            dialog_discard_confirm_key(key);
            break;

        case DIALOG_TRADE_CONFIRM:
            dialog_trade_confirm_key(key);
            break;

        case DIALOG_TALK:
            dialog_talk_key(key);
            break;

        case DIALOG_QUESTION:
            dialog_question_key(key);
            break;

        default:
            break;
        }

    }

    return dialog_mode;

}


/*
  returns nonzero if a dialog is active (game paused)
*/

int
gui_mode(void)
{
    return dialog_mode;
}




/*
  returns choosed character
*/

Character *
gui_choosed(void)
{
    if (dialog_mode)
        return choosed_character;

    return game_get_leader();
}




/*
  should we show spots?
*/

int
gui_if_show_spots(void)
{
    return gui_show_spots;
}


/*
  set choose character
*/

void
gui_set_choosed(Character *choosed)
{
    choosed_character = choosed;
    main_menu_show();
}


/*
  get gui font
*/

Pixmap **
gui_get_font(void)
{
    return gui_font;
}




/*
  player is dead dialog
*/

void
gui_player_dead(Character *who, int show_message)
{

    char message[1024];
    Character *tmpchar;


    if (show_message) {
        sprintf(message, "%s is dead.", who->name);
        gui_message(message, 1);

        while (dialog_mode == DIALOG_MESSAGE) {
            lord_poll_events();
            gui_frame();
            map_animate_frame();
            graphics_update_screen();
            lord_timer(50);
        }
    }

    tmpchar = choosed_character;
    who->life = 0;
    choosed_character = who;

    dialog_trade_to_show();
    while (dialog_mode == DIALOG_TRADE_TO || dialog_mode == DIALOG_TRADE) {
        lord_poll_events();
        gui_frame();
        map_animate_frame();
        graphics_update_screen();
        lord_timer(50);
    }

    if (who->ring_mode) {
        cartoon_play("cart10");
        exit(0);
    }

    choosed_character = tmpchar;
    quit_menu();

}




/*
  proceed all gui frames
*/
void
gui_proceed_frames(void)
{
    while (gui_frame()) {
        map_animate_frame();
        graphics_update_screen();
        lord_timer(FRAME_TIME);
        lord_poll_events();
    }
}



#ifdef TTT

/* write a text */
void
gui_draw_text(char *text, int x, int y)
{
    int i;
    for (i = 0; text[i]; ++i)
        if (gui_font[(Uint8)text[i]])
            pixmap_draw(gui_font[(Uint8)text[i]],
                        x + SCROLL_ELEMENT_SIZE * i, y);
}


/*
  dialog when starting TTT
*/

int
gui_ttt_start_dialog(void)
{
    int key;
    int i;
    int c;
    int mode = 0;
    int savegames[10];
    int numsaves = 0;
    char buf[20];
    FILE *testfile;
    int y;

    graphics_set_background("ongame");

    /* set alpha for font */
    c = gui_font[(Uint8)' ']->data[0];
    for (i = 0; i < 0x100; ++i)
        if (gui_font[i])
            pixmap_set_alpha(gui_font[i], c);

    gui_draw_text("Old or", 30, 45);
    gui_draw_text("New Game (O/N)", 30, 45 + 8);
    graphics_update_screen();

    for (i = 0; i < 10; ++i)
        savegames[i] = 0;
    numsaves = 0;

    while (!lord_key_esc()) {
        key = lord_get_key();
        if (mode == 0) {
            if (key == 'n') {
                for (i = 0; i < 10; ++i) {
                    sprintf(buf, "savegame.%d", i);
                    testfile = fopen(lord_homedir_filename(buf), "rb");
                    if (testfile != NULL) {
                        numsaves++;
                        savegames[i] = 1;
                        fclose(testfile);
                    }
                }

                if (numsaves == 0) {
                    return 0;
                }
                else {
                    mode = 1;
                    y = 68;
                    gui_draw_text("Convert from", 40, y);
                    y += 8;
                    gui_draw_text("which game?", 44, y);
                    y += 2;
                    for (i = 0; i < 10; ++i)
                        if (savegames[i]) {
                            sprintf(buf, "%d.Game %d", i, i);
                            y += 8;
                            gui_draw_text(buf, 52, y);
                        }
                    y += 8;
                    gui_draw_text("x.None", 52, y);
                    graphics_update_screen();
                }
            }
            if (key == 'o') {
                for (i = 0; i < 10; ++i) {
                    sprintf(buf, "savegame2.%d", i);
                    testfile = fopen(lord_homedir_filename(buf), "rb");
                    if (testfile != NULL) {
                        numsaves++;
                        savegames[i] = 1;
                        fclose(testfile);
                    }
                }

                if (numsaves > 0) {
                    mode = 2;
                    y = 68;
                    gui_draw_text("Which Game?", 44, y);
                    for (i = 0; i < 10; ++i)
                        if (savegames[i]) {
                            sprintf(buf, "%d.Game %d", i, i);
                            y += 8;
                            gui_draw_text(buf, 52, y);
                        }
                    graphics_update_screen();
                }
            }
        }

        if (mode == 1) {
            if (key == 'x')
                return 0;
            if (key >= '0' && key <= '9')
                if (savegames[key - '0']) {
                    game_convert(key - '0');
                    return 0;
                }
        }

        if (mode == 2) {
            if (key >= '0' && key <= '9')
                if (savegames[key - '0'])
                    return key - '0';
        }
        lord_poll_events();
        lord_timer(50);
    }

    /* unset font alpha */
    for (i = 0; i < 0x100; ++i)
        if (gui_font[i])
            gui_font[i]->hasalpha = 0;

    lord_reset_keyboard();
    return 0;

}


#endif
