/****************************************************************************
 
    archive.c
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


#include "lord.h"
#include "utils.h"
#include "archive.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



/* buffer used for decompression */
#define BUFSIZE 200000
Uint8 buffer[BUFSIZE];


/*
  opens idx'ed archive file
*/

Archive *
idxarchiveopen(const char *name)
{
    FILE *datafile;
    FILE *idxfile;
    char *fullname;             /* name + suffix */
    Archive *result;
    int i, k, n;
    Uint8 tmpbytes[2];

    fullname = addsuffix(name, "dat");
    datafile = lordfopen(fullname, "rb");
    free(fullname);

    fullname = addsuffix(name, "idx");
    idxfile = lordfopen(fullname, "rb");
    free(fullname);

    result = (Archive *) lordmalloc(sizeof(Archive));

    n = filelen(idxfile);

    result->datafile = datafile;

    n = (n + 1) / 2;
    result->size = n;

    result->index = lordmalloc((n + 1) * sizeof(int));

    result->index[0] = 0;

    for (i = 0; i < n; ++i) {
        tmpbytes[0] = fgetc(idxfile);
        tmpbytes[1] = fgetc(idxfile);

        k = tmpbytes[1] * 0x100 + tmpbytes[0];

        result->index[i + 1] = result->index[i] + k;
    }

    fclose(idxfile);

    if (result->index[n] != filelen(datafile)) {
        fprintf(stderr,
                "lord: %s seems not to be a valid idx'ed data file\n", name);
        exit(1);
    }

    return result;

}





/*
  opens ndx'ed archive file
*/

Archive *
ndxarchiveopen(const char *name)
{
    FILE *datafile;
    FILE *ndxfile;
    char *fullname;             /* name + suffix */
    Archive *result;
    int i, k, n;
    Uint8 tmpbytes[4];

    fullname = addsuffix(name, "dat");
    datafile = lordfopen(fullname, "rb");
    free(fullname);

    /* ndx file from nnpcs is npcs.ndx */
    if (strcmp(name, "nnpcs") == 0)
        name++;

    fullname = addsuffix(name, "ndx");
    ndxfile = lordfopen(fullname, "rb");
    free(fullname);

    result = (Archive *) lordmalloc(sizeof(Archive));

    n = filelen(ndxfile);

    result->datafile = datafile;

    n = (n + 3) / 4;
    result->size = n;

    result->index = lordmalloc((n + 1) * sizeof(int));

    result->index[0] = 0;

    if (n < 2 || fgetc(ndxfile) != 0
        || fgetc(ndxfile) != 0
        || fgetc(ndxfile) != 0 || fgetc(ndxfile) != 0) {
        fprintf(stderr,
                "lord: %s: expecting four zeros at the beginning of an ndx file\n",
                name);
        exit(1);
    }

    for (i = 1; i < n; ++i) {
        tmpbytes[0] = fgetc(ndxfile);
        tmpbytes[1] = fgetc(ndxfile);
        tmpbytes[2] = fgetc(ndxfile);
        tmpbytes[3] = fgetc(ndxfile);

        k = ((tmpbytes[3] * 0x100 + tmpbytes[2]) * 0x100 +
             tmpbytes[1]) * 0x100 + tmpbytes[0];


        result->index[i] = k;
        if (result->index[i - 1] > k) {
            fprintf(stderr,
                    "lord: %s seems not to be a valid ndx'ed data file\n",
                    name);
            exit(1);
        }

    }

    fclose(ndxfile);


    result->index[n] = filelen(datafile);

    if (result->index[n - 1] > result->index[n]) {
        fprintf(stderr,
                "lord: %s seems not to be a valid ndx'ed data file\n", name);
        exit(1);
    }



    return result;

}





/*
  close archive file
*/

void
archiveclose(Archive * archive)
{
    fclose(archive->datafile);
    free(archive->index);
    free(archive);
}




/*
  reads data from the archive
*/

Uint8 *
readarchive(Archive * archive, int index)
{
    Uint8 *result;
    int size;

    size = archivedatasize(archive, index);

    if (size == 0)
        return NULL;

    result = lordmalloc(size);

    fseek(archive->datafile, archive->index[index], SEEK_SET);

    if (!fread(result, size, 1, archive->datafile)) {
        fprintf(stderr, "lord: can not read datafile\n");
        exit(1);
    }

    return result;
}


/*
  return archive data size 
*/
int
archivedatasize(Archive * archive, int index)
{
    if (index < 0 || index >= archive->size)
        return 0;
    return archive->index[index + 1] - archive->index[index];
}



/*
  reads n-th bit from data
 */

inline int
readbit(Uint8 * data, int pos)
{
    Uint8 c = data[pos / 8];

    switch (pos % 8) {
    case 0:
        return (c >> 7) & 1;
    case 1:
        return (c >> 6) & 1;
    case 2:
        return (c >> 5) & 1;
    case 3:
        return (c >> 4) & 1;
    case 4:
        return (c >> 3) & 1;
    case 5:
        return (c >> 2) & 1;
    case 6:
        return (c >> 1) & 1;
    case 7:
        return (c >> 0) & 1;
    }

    return 0;                   /* should not happen onlyto avoid warnings */
}


/*
  reads n bits from data
 */

inline int
readnbits(Uint8 * data, int n, int pos)
{
    Uint8 result = 0;
    int i;

    for (i = 0; i < n; ++i)
        result = result * 2 + readbit(data, pos + i);

    return result;
}





/* 
  decompress idx data file compression
*/

Uint8 *
idxdecompress(Uint8 * data, int size, int *resultsize)
{

    Uint8 *index;
    int indexsize;
    int i;
    int datapos, bufpos;
    int codesize;
    int runlen;
    Uint8 code;
    Uint8 *result;

    if (data == NULL)
        return NULL;

    indexsize = data[0];
    index = data + 1;
    data += 1 + indexsize;

    /* now size will be the size of data in bits */
    size -= 1 + indexsize;
    size *= 8;


    /* compute the size of a single code */
    /* number af bits needed but at least 2 */

    i = indexsize;
    codesize = 0;
    while (i) {
        ++codesize;
        i = i / 2;
    }
    if (codesize < 2)
        codesize = 2;

    datapos = 0;
    bufpos = 0;


    /* idx file is coded in RLE (or some variant) */

    while (datapos + 8 <= size) {
        runlen = readnbits(data, 8, datapos);
        datapos += 8;

        /* 0 terminates the data stream */
        if (runlen == 0)
            break;

        if (runlen & 0x80) {    /* there is a repeating code */

            runlen &= 0x7f;
            code = readnbits(data, codesize, datapos);
            datapos += codesize;

            for (i = 0; i < runlen; ++i)
                buffer[bufpos++] = index[code];

        }
        else {                  /* if( runlen&0x80 ) */

            /* copy runlen codes */
            for (i = 0; i < runlen; ++i) {
                code = readnbits(data, codesize, datapos);
                datapos += codesize;
                buffer[bufpos++] = index[code];
            }
        }
    }                           /* while(datapos+8<=n*8) */

    *resultsize = bufpos;
    result = lordmalloc(*resultsize);
    memcpy(result, buffer, *resultsize);

    return result;

}


/*
  reads decompressed data from the archive
*/

Uint8 *
idxdecompressarchive(Archive * archive, int index, int *size)
{
    Uint8 *result;
    Uint8 *data;

    result = NULL;
    data = readarchive(archive, index);

    if (data != NULL) {
        result = idxdecompress(data, archivedatasize(archive, index), size);
        free(data);
    }

    if (result == NULL) {
        fprintf(stderr, "lord: archive decompression failed\n");
        exit(1);
    }

    return result;

}






/* 
  decompress ndx data file compression
*/

Uint8 *
ndxdecompress(Uint8 * data, int size, int *resultsize)
{
    Uint8 *result;
    int bufpos;
    int datapos;
    int n;
    int pattern = 0;
    int i, j;
    int start, end, len;

    bufpos = datapos = 0;

    n = 0;
    while (datapos < size) {

        if (n % 9 == 0) {
            /* every 9-th byte is a pattern for next 8 values */
            pattern = data[datapos++];
        }
        else {
            if (pattern & 1) {
                /* if the pattern bit == 1 just copy the next byte */
                buffer[bufpos++] = data[datapos++];
            }
            else {
                /* if the pattern bit == 0 we copy some substring of decoded data */

                i = (Uint8) (data[datapos++]);
                if (!(datapos < size))
                    break;
                j = (Uint8) (data[datapos++]);


                /* next 12 bites are starting position */
                start = (Uint8) (i) + 16 * (Uint8) (j & 0xf0);
                /* we add 0x12 (it is part of ndx format) */
                start = (start + 0x12) & 0xfff;

                /* next 4 bites are length-3 */
                len = (Uint8) (j & 0x0f) + 3;


                if (start > bufpos) {
                    /* it is unclear what to do in this case 
                       fprintf( stderr, "lord: wrongly decoded ndx archive %03x, %03x, %x\n", start, bufpos, len ); */

                    /* this works */

                    start -= 0x1000;
                    while (start < 0 && len > 0) {
                        buffer[bufpos++] = 0x20;
                        ++start;
                        --len;
                    }
                }

                /* 12 bites can not encode too much:
                   the real starting position 
                   is in the last segment of size 0x1000 */

                if (start < (bufpos % 0x1000))
                    start += (bufpos / 0x1000) * 0x1000;
                else if (bufpos > 0xfff)
                    start += ((bufpos / 0x1000) - 1) * 0x1000;

                end = start + len;

                for (i = start; i < end; ++i)
                    buffer[bufpos++] = buffer[i];

            }
            pattern = pattern >> 1;

        }                       /* if( datapos%9==0 ) */

        ++n;

    }


    *resultsize = bufpos;
    result = lordmalloc(*resultsize);
    memcpy(result, buffer, *resultsize);

    return result;

}


/*
  reads decompressed data from the archive
*/

Uint8 *
ndxdecompressarchive(Archive * archive, int index, int *size)
{
    Uint8 *result;
    Uint8 *data;

    result = NULL;
    data = readarchive(archive, index);

    if (data != NULL) {
        result = ndxdecompress(data, archivedatasize(archive, index), size);
        free(data);
    }

    if (result == NULL) {
        fprintf(stderr, "lord: archive decompression failed index=%d\n",
                index);
        exit(1);
    }

    return result;

}
