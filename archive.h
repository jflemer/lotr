/****************************************************************************

    archive.h
    provides access to idx'ed and ndx'ed data files


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


#ifndef _ARCHIVE_H
#define _ARCHIVE_H

#include "lord.h"
#include <stdio.h>



/* indexed archive file  */
typedef struct
{
    FILE *datafile;
    int *index;
    int size;
} Archive;


/* opens idx'ed archive file */
extern Archive *idxarchiveopen(const char *name);

/* opens ndx'ed archive file */
extern Archive *ndxarchiveopen(const char *name);

/* close archive file */
extern void archiveclose(Archive *archive);

/* reads data from the archive */
extern Uint8 *readarchive(Archive *archive, int index);

/* return archive data size */
extern int archivedatasize(Archive *archive, int index);

/* decompress idx data file compression */
extern Uint8 *idxdecompress(Uint8 *data, int size, int *resultsize);

/* reads decompressed data from the archive */
extern Uint8 *idxdecompressarchive(Archive *archive, int index, int *size);

/* decompress ndx data file compression */
extern Uint8 *ndxdecompress(Uint8 *data, int size, int *resultsize);

/* reads decompressed data from the archive */
extern Uint8 *ndxdecompressarchive(Archive *archive, int index, int *size);


#endif /* _ARCHIVE_H */
