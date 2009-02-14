/****************************************************************************

    av.c
    play av file


    Lord of the Rings game engine

    Copyright (C) 2004  Michal Benes

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
#include "av.h"
#include "graphics.h"
#include "lord_sdl.h"
#include "timing.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define AV_WIDTH 28
#define AV_HEIGHT 18
#define AV_SAMPLE_LEN 732



char *av_names[] = { "sacf",
    "sadf",
    "sagf",
    "saaf",
    "sahf",
    "saif",
    "sajf",
    "sabf",
    "saef",
    "sakf",
    "salf",
    "samf",
    "sanf",
    "saff",
    "saof",
    "sarf",
    "saqf"
};

/*
   play av
*/
//TODO robust freads
void
playav(char *name)
{
    FILE *avfile;
    char *fullname;             /* name + suffix */
    Uint8 index[2 * AV_WIDTH * AV_HEIGHT];
    Uint8 sample[AV_SAMPLE_LEN];
    Palette palette;
    Uint8 *src, *dst;

    Uint8 *audiobuf;
    int audiobufsize = 0;
    int audiobufpos = 0;

    Pixmap *pixmap;
    Pixmap *av_frame, *av_frame_old, *av_frame_tmp;
    long avlen;
    int i, j;
    int x, y, fr, av_frame_num;
    int k;
    int pos;

    fullname = addsuffix(name, "av");
    avfile = lordfopen(fullname, "rb");
    free(fullname);


    avlen = filelen(avfile);

    if (avlen < 0x300 + 2 * AV_WIDTH * AV_HEIGHT + AV_SAMPLE_LEN * 14) {
        fprintf(stderr, "lord: corrupted av file %s.av len=%ld\n", name,
                avlen);
        exit(1);
    }



    SetBackground("vid");



    fread(&palette, 0x300, 1, avfile);
    SetPalette(&palette, 9, 0x100 - 9);


    pixmap = pixmap_new(8, 8);
    av_frame = pixmap_new(8 * AV_WIDTH, 8 * AV_HEIGHT);
    av_frame_old = pixmap_new(8 * AV_WIDTH, 8 * AV_HEIGHT);

    audiobufsize = 60 * AV_SAMPLE_LEN;
    audiobuf = lordmalloc(audiobufsize);

    for (i = 0; i < 14; ++i) {
        fread(sample, AV_SAMPLE_LEN, 1, avfile);
        memcpy(audiobuf + audiobufpos, sample, AV_SAMPLE_LEN);
        audiobufpos += AV_SAMPLE_LEN;
    }

    PlaySample(audiobuf, audiobufpos);

    fread(index, 2 * AV_WIDTH * AV_HEIGHT, 1, avfile);



    ResetKeyboard();
    ResetTimer();




    av_frame_num = 0;
    while (!KeyEsc()) {


        /* draw new 8x8 squares */

        for (j = 0; j < AV_HEIGHT; ++j)
            for (i = 0; i < AV_WIDTH; ++i) {
                x = index[(j * AV_WIDTH + i) * 2];
                y = index[(j * AV_WIDTH + i) * 2 + 1];
                if (x == 0xff && y == 0xff) {


                    fread(pixmap->data, 1, 8 * 8, avfile);

                    src = pixmap->data;
                    dst = av_frame->data + j * AV_WIDTH * 8 * 8 + i * 8;

                    for (k = 0; k < 8; ++k) {
                        memcpy(dst, src, 8);
                        dst += AV_WIDTH * 8;
                        src += 8;
                    }

                }
            }



        /* recycle old 8x8 squares */


        for (j = 0; j < AV_HEIGHT; ++j)
            for (i = 0; i < AV_WIDTH; ++i) {
                x = index[(j * AV_WIDTH + i) * 2];
                y = index[(j * AV_WIDTH + i) * 2 + 1];
                if (x != 0xff || y != 0xff) {
                    pos = 32 * 8 * y + x;
                    x = pos % (AV_WIDTH * 8);
                    y = pos / (AV_WIDTH * 8) / 2;

                    fr = (pos / (AV_WIDTH * 8) + av_frame_num) % 2;

                    if ((y + 7) * AV_WIDTH * 8 + x + 7 <
                        AV_HEIGHT * AV_WIDTH * 8 * 8) {

                        if (fr == 0)
                            src = av_frame->data + y * AV_WIDTH * 8 + x;
                        else
                            src = av_frame_old->data + y * AV_WIDTH * 8 + x;
                        dst = av_frame->data + j * AV_WIDTH * 8 * 8 + i * 8;

                        for (k = 0; k < 8; ++k) {
                            memmove(dst, src, 8);
                            dst += AV_WIDTH * 8;
                            src += AV_WIDTH * 8;
                        }
                    }
                    else {
                        fprintf(stderr,
                                "lord: corrupted av file (x=%d, y=%d)\n", x,
                                y);
                        exit(1);
                    }
                }
            }


        pixmap_draw(av_frame, 53, 24);
        UpdateScreen();
        Timer(66);
        PollEvents();

        //      while( !KbHit() ) PollEvents(); GetKey();


        if (fread(sample, AV_SAMPLE_LEN, 1, avfile) != 1)
            break;


        if (audiobufpos + AV_SAMPLE_LEN > audiobufsize) {

            audiobufsize += 60 * AV_SAMPLE_LEN;

            if ((audiobuf = realloc(audiobuf, audiobufsize)) == NULL) {
                fprintf(stderr, "lord: can not allocate memory.\n");
                exit(1);
            }
        }
        memcpy(audiobuf + audiobufpos, sample, AV_SAMPLE_LEN);
        audiobufpos += AV_SAMPLE_LEN;
        ResizeSample(audiobuf, audiobufpos);

        if (fread(index, 2 * AV_WIDTH * AV_HEIGHT, 1, avfile) != 1)
            break;

        av_frame_tmp = av_frame_old;
        av_frame_old = av_frame;
        av_frame = av_frame_tmp;

        av_frame_num++;

    }

    while (PlayingSample() && !KeyEsc()) {
        Timer(10);
        PollEvents();
    }
    ResizeSample(NULL, 0);

    free(audiobuf);

    pixmap_free(pixmap);
    pixmap_free(av_frame);
    pixmap_free(av_frame_old);

    ResetKeyboard();

}




/*
  play av by number
*/

void
playavnum(int num)
{
    if (num < 0 || num >= 17) {
        fprintf(stderr, "lord: non-existent av file number=%d\n", num);
        exit(1);
    }

    playav(av_names[num]);

}
