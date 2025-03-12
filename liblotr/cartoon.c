/****************************************************************************

    cartoon.c
    cartoon playing


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
#include "timing.h"
#include "graphics.h"
#include "cartoon.h"
#include "archive.h"
#include "credit.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




/* should we debug cartoon commands? */
#define DEBUGCARTOONCOMMANDS 0

/* maximum nuber of resources (pixmaps, fonts) */
#define MAXRESOURCES 256

/* maximal length of text - without terminating zero */
#define TEXTMAX (1024-1)


/* show Programming by Michal Benes */
int showcredit = 0;



/* cartoon decription create */
CartoonDesc *cartoondesc_create(Uint8 *data, int size);

/* cartoon decription next command */
CartoonCommand *cartoondesc_next(CartoonDesc *cartoondesc);



/*
   check that register has valid range
*/

void
checkregister(int i)
{
    if (i < 0 || i > MAXRESOURCES) {
        fprintf(stderr, "lotr: register is in invalid ranga\n");
        exit(1);
    }
}


/*
  check command length
 */

void
checkcommandsize(CartoonCommand * command, int size)
{
    if (command->size != size) {
        fprintf(stderr,
                "lotr: command %02x length should be %d (%d found)\n",
                command->type, size, command->size);
        exit(1);
    }
}



/*
  check command length is at least size
 */

void
checkcommandsizeatleast(CartoonCommand * command, int size)
{
    if (command->size < size) {
        fprintf(stderr,
                "lotr: command %02x length should be at least %d (%d found)\n",
                command->type, size, command->size);
        exit(1);
    }
}


void
printcommand(CartoonCommand * command)
{

    int i;

    printf(">>> cartoon command type=%04x\n", command->type);
    printf("hex: ");
    for (i = 0; i < command->size; ++i) {
        printf("%04x ", command->data[i]);
    }
    /*
       printf( "\ndec: " );
       for( i=0; i<command->size; ++i )
       {
       printf( "%04d ", command->data[i] );
       }
     */
    printf("\n");
}



static inline int
get_command_par(const CartoonCommand * command, const int *registers, int index)
{
    int a, b;
    a = command->data[index];
    b = command->data[index + 1];
    if (b == 0x100)
        return registers[a];

    if (b >= 0x80)
        return (a + 0x100 * b) - 0x10000;

    return a + 0x100 * b;
}


/*
  play cartoon
*/

extern void
cartoon_play(char *name)
{
    Archive *archive;
    int archivepos = 0;
    Uint8 *data;
    int size;
    CartoonDesc *cartoondesc;
    CartoonCommand *command;
    int i, j, k, l;
    int repeats;
    Pixmap *pixmaps[MAXRESOURCES];
    CartoonFont *fonts[MAXRESOURCES];
    int registers[MAXRESOURCES];
    int counters[MAXRESOURCES];
    int returns[MAXRESOURCES];
    int return_pos;
    Pixmap *creditpixmap = NULL;
    char text[TEXTMAX + 1];
    /* double buffer screen */
    Pixmap *bufscreen;


    if (showcredit) {
        creditpixmap = pixmap_new(100, 16);
        for (i = 0; i < 100 * 16; ++i)
            creditpixmap->data[i] = credit_data[i];
    }


    archive = archive_idx_open(name);

    /* initialize the resources cache */
    for (i = 0; i < MAXRESOURCES; ++i) {
        fonts[i] = NULL;
        pixmaps[i] = NULL;
        registers[i] = 0;
        counters[i] = 0;
    }


    /* initialize double buffering screen */
    bufscreen = pixmap_new(SCREEN_WIDTH, SCREEN_HEIGHT);


    graphics_clear_screen();
    graphics_update_screen();

    while (archivepos < archive->size && !lotr_key_esc()) {
        data = decompress_idxarchive(archive, archivepos, &size);
        /* data are freed by cartoondesc_free */

        /* end if do not find next archive command */
        archivepos = archive->size;


        /* resets return_pos */
        return_pos = 0;

        repeats = 0;

        /* TODO cartoons are 320x200 center them on the screen */

        /* reset screen */
        graphics_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        bzero(bufscreen->data, SCREEN_WIDTH * SCREEN_HEIGHT);
        graphics_clear_screen();
        graphics_update_screen();


        cartoondesc = cartoondesc_create(data, size);
        free(data);


        lotr_reset_timer();

        while ((command = cartoondesc_next(cartoondesc)) != NULL
               && !lotr_key_esc()) {

            lotr_poll_events();

            if (DEBUGCARTOONCOMMANDS) {
                printf("\n>>> %x (gosub level=%d) ", cartoondesc->pos - 1,
                       return_pos);
                printcommand(command);
            }


            switch (command->type) {



              /****************************************************/
                case 0x00:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Relax\n");

                    checkcommandsize(command, 0);

                    break;




              /****************************************************/
                case 0x02:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Wait for a key\n");

                    checkcommandsize(command, 0);

                    while (!lotr_kb_hit());

                    break;




              /****************************************************/
                case 0x03:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Clear area\n");

                    checkcommandsize(command, 12);

                    graphics_clear_area(get_command_par
                                        (command, registers, 0),
                                        get_command_par(command, registers,
                                                        2),
                                        get_command_par(command, registers,
                                                        4),
                                        get_command_par(command, registers,
                                                        6));

                    graphics_update_screen();

                    break;



              /****************************************************/
                case 0x2c:
                    lotr_timer(10);
                    break;


              /****************************************************/
                case 0x27:
                case 0x20:
                case 0x04:

                    /* this is probably completely wrong */

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> ?Timing function (Unknown)?\n");

                    checkcommandsize(command, 2);

                    i = get_command_par(command, registers, 0);

                    if (i > 0 && command->type == 0x04)
                        lotr_sleep(i * 5);

                    if (i < 0 && command->type == 0x27)
                        lotr_sleep(-i * 5);



                    break;


              /****************************************************/
                case 0x05:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Set repeats\n");

                    checkcommandsize(command, 2);

                    repeats = get_command_par(command, registers, 0);

                    break;



              /****************************************************/
                case 0x06:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Repeat\n");

                    checkcommandsize(command, 1);

                    if ((repeats--) > 0)
                        cartoondesc_goto(cartoondesc, command->data[0]);

                    break;



              /****************************************************/
                case 0x08:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Load font\n");

                    checkcommandsize(command, 3);

                    /* cache index */
                    i = get_command_par(command, registers, 1);

                    checkregister(i);
                    if (fonts[i] != NULL) {
                        fprintf(stderr,
                                "lotr: can not store font resource %d\n", i);
                        exit(1);
                    }
                    fonts[i] = cartoon_font_read(archive, command->data[0]);


                    break;



              /****************************************************/
                case 0x09:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Write text\n");

                    checkcommandsizeatleast(command, 9);

                    for (i = 8, j = 0; i < command->size && j < TEXTMAX; ++i) {
                        text[j] = command->data[i] & 0xff;
                        if (text[j++] == 0)
                            break;
                        text[j] = command->data[i] / 0x100;
                        if (text[j++] == 0)
                            break;
                    }

                    /* terminate text if it was not terminated before */
                    text[TEXTMAX] = 0;

                    cartoon_font_write_text(fonts[get_command_par(command, registers, 0)],      /* font used */
                                            get_command_par(command, registers, 2),     /* x-pos */
                                            get_command_par(command, registers, 4),     /* y-pos */
                                            text);

                    graphics_update_screen();

                    break;


              /****************************************************/
                case 0x0a:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> set register\n");

                    checkcommandsize(command, 4);

                    /* cache index */
                    i = get_command_par(command, registers, 0);

                    checkregister(i);

                    registers[i] = get_command_par(command, registers, 2);


                    break;



              /****************************************************/
                case 0x0b:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> add to register\n");

                    checkcommandsize(command, 4);

                    /* cache index */
                    i = get_command_par(command, registers, 0);

                    checkregister(i);

                    registers[i] += get_command_par(command, registers, 2);


                    break;





              /****************************************************/
                case 0x0c:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> substract from register\n");

                    checkcommandsize(command, 4);

                    /* cache index */
                    i = get_command_par(command, registers, 0);

                    checkregister(i);

                    registers[i] -= get_command_par(command, registers, 2);


                    break;




              /****************************************************/
                case 0x0d:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Goto\n");

                    checkcommandsize(command, 1);

                    cartoondesc_goto(cartoondesc, command->data[0]);

                    break;




              /****************************************************/
                case 0x10:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> if register==number\n");

                    checkcommandsize(command, 5);

                    /* cache index */
                    i = get_command_par(command, registers, 0);

                    checkregister(i);

                    if (registers[i] ==
                        get_command_par(command, registers, 2))
                        cartoondesc_goto(cartoondesc, command->data[4]);


                    break;



              /****************************************************/
                case 0x11:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> store keycode to register\n");

                    checkcommandsize(command, 2);

                    /* cache index */
                    i = get_command_par(command, registers, 0);

                    checkregister(i);

                    registers[i] = lotr_get_key();


                    break;




              /****************************************************/
                case 0x12:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Load pixmap from the screen\n");

                    checkcommandsize(command, 10);

                    i = get_command_par(command, registers, 0);
                    checkregister(i);
                    if (pixmaps[i] != NULL) {
                        fprintf(stderr,
                                "lotr: can not store pixmap resource %d\n",
                                i);
                        exit(1);
                    }

                    pixmaps[i] =
                        pixmap_subscreen(get_command_par
                                         (command, registers, 2),
                                         get_command_par(command, registers,
                                                         4),
                                         get_command_par(command, registers,
                                                         6),
                                         get_command_par(command, registers,
                                                         8));

                    break;




              /****************************************************/
                case 0x14:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Save screen to screen buffer\n");

                    checkcommandsize(command, 0);

                    graphics_save_screen(bufscreen->data);

                    break;



              /****************************************************/
                case 0x15:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Show screen buffer\n");

                    checkcommandsize(command, 0);

                    pixmap_draw(bufscreen, 0, 0);
                    graphics_update_screen();

                    break;





              /****************************************************/
                case 0x16:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Load pixmap\n");

                    checkcommandsize(command, 5);

                    /* cache index */
                    i = get_command_par(command, registers, 1);
                    checkregister(i);

                    if (pixmaps[i] != NULL) {
                        fprintf(stderr,
                                "lotr: can not store pixmap resource %d\n",
                                i);
                        exit(1);
                    }

                    pixmaps[i] =
                        pixmap_read_from_idx_archive(archive,
                                                     command->data[0]);

                    /* alpha value */
                    if (command->data[3] < 0xff)
                        pixmap_set_alpha(pixmaps[i], command->data[3]);


                    /* show used pixmaps
                       graphics_clear_screen();
                       graphics_fade_palette( 100, 0, 0x100 );
                       printf( "%d\n", i );
                       pixmap_draw( pixmaps[i], 0, 0 );
                       graphics_update_screen();
                       lotr_reset_keyboard();
                       while( !lotr_kb_hit() );
                       lotr_reset_keyboard();
                     */


                    break;





              /****************************************************/
                case 0x17:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Load palette\n");

                    checkcommandsize(command, 1);

                    data =
                        decompress_idxarchive(archive, command->data[0],
                                              &size);

                    if (size != sizeof(Palette)) {
                        fprintf(stderr,
                                "lotr: trying to load an invalid palette\n");
                        exit(1);
                    }

                    graphics_set_palette((Palette *)data, 0, 0x100);
                    free(data);

                    graphics_update_screen();

                    break;





              /****************************************************/
                case 0x13:     /* draws pixmap */
                case 0x18:


                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Draw pixmap\n");

                    checkcommandsize(command, 6);

                    /* wait if we are drawing large pixmaps */
                    lotr_timer(0);

                    /* cache index */
                    i = get_command_par(command, registers, 0);
                    checkregister(i);

                    if (pixmaps[i] == NULL) {
                        fprintf(stderr,
                                "lotr: cartoon attemps to draw an empty pixmap\n");
                        break;
                        /* exit(1); */
                    }


                    if (command->type == 0x18) {        /* draw to buffer */
                        pixmap_draw_to_buffer(bufscreen->data, pixmaps[i],
                                              get_command_par(command,
                                                              registers, 2),
                                              get_command_par(command,
                                                              registers, 4));
                    } else {
                        pixmap_draw(pixmaps[i],
                                    get_command_par(command, registers, 2),
                                    get_command_par(command, registers, 4));

                        if (showcredit) {
                            showcredit = 0;
                            pixmap_draw(creditpixmap, 200, 180);
                            pixmap_free(creditpixmap);
                        }

                        graphics_update_screen();
                    }


                    /* wait if we are drawing large pixmaps */
                    lotr_timer(pixmaps[i]->datasize / 300);


                    break;




              /****************************************************/
                case 0x1a:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Free pixmap\n");

                    checkcommandsize(command, 2);

                    i = get_command_par(command, registers, 0);
                    checkregister(i);

                    if (pixmaps[i] == NULL) {
                        fprintf(stderr,
                                "lotr: can not free NULL pixmap resource %d\n",
                                i);
                        exit(1);
                    }

                    pixmap_free(pixmaps[i]);
                    pixmaps[i] = NULL;

                    break;



              /****************************************************/
                case 0x1c:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Load pixmap from a screen buffer\n");

                    checkcommandsize(command, 10);

                    i = get_command_par(command, registers, 0);
                    checkregister(i);
                    if (pixmaps[i] != NULL) {
                        fprintf(stderr,
                                "lotr: can not store pixmap resource %d\n",
                                i);
                        exit(1);
                    }


                    /* hack to fix Fatty Bolger cartoon */
                    if (!strcmp(name, "cart2") && archivepos == 19 &&
                        command->data[8] == 0x64)
                        command->data[8] = 0x47;

                    pixmaps[i] = pixmap_subpixmap(bufscreen,
                                                  get_command_par(command,
                                                                  registers,
                                                                  2),
                                                  get_command_par(command,
                                                                  registers,
                                                                  4),
                                                  get_command_par(command,
                                                                  registers,
                                                                  6),
                                                  get_command_par(command,
                                                                  registers,
                                                                  8));

                    break;




              /****************************************************/
                case 0x1d:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Load pixmap from a pixmap\n");

                    checkcommandsize(command, 12);

                    i = get_command_par(command, registers, 2);
                    checkregister(i);

                    if (pixmaps[i] != NULL) {
                        fprintf(stderr,
                                "lotr: can not store pixmap resource %d\n",
                                i);
                        exit(1);
                    }

                    j = get_command_par(command, registers, 0);
                    checkregister(j);

                    if (pixmaps[j] == NULL) {
                        fprintf(stderr,
                                "lotr: can not subpixmap NULL pixmap resource %d\n",
                                j);
                        exit(1);
                    }



                    pixmaps[i] = pixmap_subpixmap(pixmaps[j],
                                                  get_command_par(command,
                                                                  registers,
                                                                  4),
                                                  get_command_par(command,
                                                                  registers,
                                                                  6),
                                                  get_command_par(command,
                                                                  registers,
                                                                  8),
                                                  get_command_par(command,
                                                                  registers,
                                                                  10));

                    break;





              /****************************************************/
                case 0x1e:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Gosub\n");

                    checkcommandsize(command, 1);

                    if (return_pos == MAXRESOURCES) {
                        fprintf(stderr,
                                "lotr: too much nested cartoon gosubs\n");
                        exit(1);
                    }

                    returns[return_pos++] = cartoondesc->pos;
                    cartoondesc_goto(cartoondesc, command->data[0]);


                    break;





              /****************************************************/
                case 0x1f:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Return\n");

                    checkcommandsize(command, 0);

                    if (return_pos == 0) {
                        fprintf(stderr,
                                "lotr: cartoon return without gosub\n");
                        exit(1);
                    }


                    cartoondesc_goto(cartoondesc, returns[--return_pos]);


                    break;




              /****************************************************/
                case 0x21:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> set counter\n");

                    checkcommandsize(command, 4);

                    i = get_command_par(command, registers, 0);
                    j = get_command_par(command, registers, 2);
                    checkregister(i);

                    counters[i] = j;


                    break;



              /****************************************************/
                case 0x22:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> -- counter\n");

                    checkcommandsize(command, 3);

                    i = get_command_par(command, registers, 0);
                    checkregister(i);

                    if (--counters[i] < 0)
                        cartoondesc_goto(cartoondesc, command->data[2]);


                    break;




              /****************************************************/
                case 0x23:     /* rotates colors */

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Rotate palette\n");

                    checkcommandsize(command, 6);

                    i = get_command_par(command, registers, 0);
                    j = get_command_par(command, registers, 2);

                    if (get_command_par(command, registers, 4) % 2)
                        graphics_rotate_palette_left(i, j);
                    else
                        graphics_rotate_palette_right(i, j);

                    graphics_update_screen();

                    break;



              /****************************************************/
                case 0x24:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Random number\n");

                    checkcommandsize(command, 6);

                    i = get_command_par(command, registers, 0);
                    checkregister(i);

                    k = get_command_par(command, registers, 2);
                    l = get_command_par(command, registers, 4);


                    if (l <= k)
                        l = k;

                    registers[i] = k + random() % (l - k + 1);

                    break;





              /****************************************************/
                case 0x25:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> Load new cartoon description\n");

                    checkcommandsize(command, 1);

                    archivepos = command->data[0];
                    cartoondesc_finish(cartoondesc);

                    break;



              /****************************************************/
                case 0x28:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> fade palette\n");

                    checkcommandsize(command, 2);

                    i = get_command_par(command, registers, 0);
                    graphics_fade_palette(i, 0, 0x100);
                    graphics_update_screen();

                    break;





              /****************************************************/
                case 0x29:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> if register>=number\n");

                    checkcommandsize(command, 5);

                    /* cache index */
                    i = get_command_par(command, registers, 0);

                    checkregister(i);

                    if (registers[i] >=
                        get_command_par(command, registers, 2))
                        cartoondesc_goto(cartoondesc, command->data[4]);


                    break;



              /****************************************************/
                case 0x2a:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> if register<=number\n");

                    checkcommandsize(command, 5);

                    /* cache index */
                    i = get_command_par(command, registers, 0);

                    checkregister(i);

                    if (registers[i] <=
                        get_command_par(command, registers, 2))
                        cartoondesc_goto(cartoondesc, command->data[4]);


                    break;



              /****************************************************/
                case 0x2b:

                    if (DEBUGCARTOONCOMMANDS)
                        printf(">>> if register!=number\n");

                    checkcommandsize(command, 5);

                    /* cache index */
                    i = get_command_par(command, registers, 0);

                    checkregister(i);

                    if (registers[i] !=
                        get_command_par(command, registers, 2))
                        cartoondesc_goto(cartoondesc, command->data[4]);


                    break;



                default:
                    fprintf(stderr, "lotr: unknown cartoon command %04x\n",
                            command->type);
                    /* exit(1); */
            }

            lotr_timer(0);

            if (0 && return_pos == 0) {
                lotr_reset_keyboard();
                while (!lotr_kb_hit());
                lotr_reset_keyboard();
            }


        }                       /*  while( cartoondesc_next )  */

        cartoondesc_free(cartoondesc);

        /* free cached resources */
        for (i = 0; i < MAXRESOURCES; ++i) {
            if (fonts[i] != NULL) {
                cartoon_font_free(fonts[i]);
                fonts[i] = NULL;
            }
            if (pixmaps[i] != NULL) {
                pixmap_free(pixmaps[i]);
                pixmaps[i] = NULL;
            }
        }

    }                           /* while( archivepos<archive->size ) */

    /* free resources */

    pixmap_free(bufscreen);
    archive_close(archive);

    lotr_reset_keyboard();

}





/*
  cartoon decription create
*/

CartoonDesc *
cartoondesc_create(Uint8 *data, int size)
{
    CartoonDesc *result;
    int i, commandnum;
    CartoonCommand *command;


    result = lotr_malloc(sizeof(CartoonDesc));


    for (i = 0; i < MAXCOMMANDS; ++i)
        result->commands[i] = NULL;

    commandnum = 0;
    for (i = 0; i < size - 2 && commandnum < MAXCOMMANDS;) {
        command = lotr_malloc(sizeof(CartoonCommand));

        command->type = data[i] + data[i + 1] * 0x100;
        i += 2;
        command->size = 0;

        while (i < size - 2 && (data[i] != 0xfe || data[i + 1] != 0xff)
               && command->size < MAXCOMMANDSIZE) {
            command->data[command->size++] = data[i] + data[i + 1] * 0x100;
            i += 2;
        }

        if (command->size == MAXCOMMANDSIZE) {
            fprintf(stderr, "lotr: too long comands in the cartoon file\n");
            exit(1);
        }

        i += 2;

        if (DEBUGCARTOONCOMMANDS) {
            printf("%x ", commandnum);
            printcommand(command);
        }

        result->commands[commandnum++] = command;
    }

    if (commandnum == MAXCOMMANDS) {
        fprintf(stderr, "lotr: too much comands in the cartoon file\n");
        exit(1);
    }

    result->pos = 0;
    result->size = commandnum;

    return result;
}






/*
   cartoon decription free
*/

void
cartoondesc_free(CartoonDesc *cartoondesc)
{
    int i;
    for (i = 0; i < cartoondesc->size; ++i)
        free(cartoondesc->commands[i]);
    free(cartoondesc);
}




/*
  cartoon decrition goto command
*/

void
cartoondesc_goto(CartoonDesc *cartoondesc, int pos)
{
    if (pos < 0)
        return;
    cartoondesc->pos = pos;
}




/*
  cartoon decription next command
*/

CartoonCommand *
cartoondesc_next(CartoonDesc *cartoondesc)
{
    if (cartoondesc->pos >= MAXCOMMANDS)
        return NULL;
    return cartoondesc->commands[cartoondesc->pos++];
}


/*
  cartoon decription finish (jumps to the end)
*/

void
cartoondesc_finish(CartoonDesc *cartoondesc)
{
    cartoondesc->pos = cartoondesc->size;
}




/*
  set if we should show credits
*/

void
cartoon_show_credits(int show_credits)
{
    showcredit = show_credits;
}
