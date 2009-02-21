/****************************************************************************

    gui.h
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


#ifndef _GUI_H
#define _GUI_H

#include "character.h"


/* type of displayed dialog */
#define DIALOG_NONE          0
#define DIALOG_OPTIONS       1
#define DIALOG_MESSAGE       2
#define DIALOG_MESSAGE_YN    3
#define DIALOG_SAVE          4
#define DIALOG_LOAD          5
#define DIALOG_BOOK          6
#define DIALOG_PARAGRAPH     7
#define DIALOG_SPOT_PRINT    8
#define DIALOG_DIED          9


#define MAIN_MENU              100
#define DIALOG_USE_MAIN        111
#define DIALOG_TRADE_TO        112
#define DIALOG_WIZARD          115

#define DIALOG_ATTACK          120
#define DIALOG_VIEW            121
#define DIALOG_USE             122
#define DIALOG_TRADE           123
#define DIALOG_TRADE_CONFIRM   124
#define DIALOG_DISCARD         125
#define DIALOG_DISCARD_CONFIRM 126
#define DIALOG_SKILL           127
#define DIALOG_MAGIC           128

/* if in combat mode, some options are disabled */
#define DISABLE_IN_COMBAT  200

#define DIALOG_TALK_MAIN   201
#define DIALOG_TALK_WHO    202

#define DIALOG_GET         210
#define DIALOG_RECRUIT     211
#define DIALOG_DISMISS     212
#define DIALOG_TALK        213
#define DIALOG_QUESTION    214
#define DIALOG_LEADER      215
#define DIALOG_CHOOSE      216





/* init gui */
extern void gui_init(void);

/* close gui */
extern void gui_close(void);

/* set the gui palette */
extern void gui_set_palette(void);

/* clears screen and draws the bounding chain */
extern void gui_clear(void);

/* set the map drawing area */
extern void gui_set_map_area(int *width, int *height);

/* set the window over whole screen */
extern void gui_set_full_window(void);

/* gui frame - returns nonzero if a dialog is active (game paused) */
extern int gui_frame(void);

/* returns nonzero if a dialog is active (game paused) */
extern int gui_mode(void);

/* should we show spots */
extern int gui_if_show_spots(void);

/* quits all menus */
extern void quit_menu();

/* draw main menu */
extern void main_menu_draw();

/* shows a message */
extern void gui_message(const char *text, int small_window);

/* shows a question */
extern void gui_question(char *text);

/* shows a paragraph */
extern void gui_paragraph(int num);

/* shows a question paragraph */
extern void gui_paragraph_question(int num);

/* shows dialog saying that the player is dead */
extern void gui_died_show(const char *text);

#ifdef TTT
/* shows a book with the next chapter in TTT */
extern void gui_book(char *text);
#endif

/* returns choosed character */
extern Character *gui_choosed(void);

/* set choose character */
extern void gui_set_choosed(Character *choosed);

/* get gui font */
extern Pixmap **gui_get_font(void);

/* player is dead dialog */
extern void gui_player_dead(Character *who, int show_message);

/* proceed all gui frames */
extern void gui_proceed_frames(void);


#ifdef TTT
/* dialog when starting TTT */
extern int gui_ttt_start_dialog(void);
#endif



#endif /* _GUI_H */
