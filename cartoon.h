/****************************************************************************

    cartoon.h
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


#ifndef _CARTOON_H
#define _CARTOON_H


#define MAXCOMMANDS 1024
#define MAXCOMMANDSIZE 256

typedef struct
{
    int type;
    int size;
    int data[MAXCOMMANDSIZE];
} CartoonCommand;


typedef struct
{
    int pos;
    int size;
    CartoonCommand *commands[MAXCOMMANDS];
} CartoonDesc;




/* play cartoon */
extern void playcartoon(char *name);

/* cartoon decription free */
extern void cartoondesc_free(CartoonDesc *cartoondesc);

/* cartoon decrition goto command */
extern void cartoondesc_goto(CartoonDesc *cartoondesc, int pos);

/* cartoon decription finish (jumps to the end) */
extern void cartoondesc_finish(CartoonDesc *cartoondesc);



/* set if we should show credits */
extern void cartoon_show_credits(int show_credits);


#endif /* _CARTOON_H */
