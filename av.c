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

// Longest sample in AV file 2208444 (in 11khz)
#define MAX_SAMPLE_GROWTH (6 * 1024 * 1024)

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
  play one audio sample
 */
static int
read_av_audio_sample(Uint8 *audiobuf, int *audiobufpos, FILE *avfile)
{
    Uint8 sample[AV_SAMPLE_LEN];
    Uint8 *buf;
    int i;

    if (fread(sample, AV_SAMPLE_LEN, 1, avfile) != 1)
        return 0;

    if (*audiobufpos + 2 * AV_SAMPLE_LEN > MAX_SAMPLE_GROWTH) {
        fprintf(stderr, "lord: Too long AV video\n");
        exit(1);
    }

    buf = audiobuf + *audiobufpos;

    // AV samples are in 11khz but other sounds in 22khz, we need to resample
    *(buf++) = sample[0];
    *(buf++) = sample[0];
    for (i = 1; i < AV_SAMPLE_LEN; ++i) {
        *(buf++) = (sample[i - 1] + sample[i]) / 2;
        *(buf++) = sample[i];
    }
    buf = audiobuf + *audiobufpos;

    *audiobufpos += 2 * AV_SAMPLE_LEN;
    return 1;
}


/*
   play av
*/
void
playav(char *name)
{
    FILE *avfile;
    char *fullname;             /* name + suffix */
    Uint8 index[2 * AV_WIDTH * AV_HEIGHT];
    Palette palette;
    Uint8 *src, *dst;

    Uint8 *audiobuf;
    int audiobufpos = 0;

    Pixmap *pixmap;
    Pixmap *av_frame, *av_frame_old, *av_frame_tmp;
    long avlen;
    int i, j;
    int x, y, fr, av_frame_num;
    int k;
    int pos;

    fullname = addsuffix(name, "av");
    avfile = lord_fopen(fullname, "rb");
    free(fullname);


    avlen = filelen(avfile);

    if (avlen < 0x300 + 2 * AV_WIDTH * AV_HEIGHT + AV_SAMPLE_LEN * 14) {
        fprintf(stderr, "lord: corrupted av file %s.av len=%ld\n", name,
                avlen);
        exit(1);
    }

    SetBackground("vid");

    if (fread(&palette, 0x300, 1, avfile) != 1) {
        fprintf(stderr, "lord: corrupted av file %s.av: can't read palette\n",
                name);
        exit(1);
    }
    SetPalette(&palette, 9, 0x100 - 9);


    pixmap = pixmap_new(8, 8);
    av_frame = pixmap_new(8 * AV_WIDTH, 8 * AV_HEIGHT);
    av_frame_old = pixmap_new(8 * AV_WIDTH, 8 * AV_HEIGHT);

    audiobuf = lord_malloc(MAX_SAMPLE_GROWTH);
    bzero(audiobuf, MAX_SAMPLE_GROWTH);

    for (i = 0; i < 14; ++i) {
        if (!read_av_audio_sample(audiobuf, &audiobufpos, avfile)) {
            fprintf(stderr,
                    "lord: corrupted av file %s.av: can't read audio data\n",
                    name);
            exit(1);
        }
    }

    play_sample(audiobuf, MAX_SAMPLE_GROWTH);

    if (fread(index, 2 * AV_WIDTH * AV_HEIGHT, 1, avfile) != 1) {
        fprintf(stderr,
                "lord: corrupted av file %s.av: can't read first frame\n",
                name);
        exit(1);
    }

    lord_reset_keyboard();
    ResetTimer();

    av_frame_num = 0;
    while (!lord_key_esc()) {

        /* draw new 8x8 squares */
        for (j = 0; j < AV_HEIGHT; ++j)
            for (i = 0; i < AV_WIDTH; ++i) {
                x = index[(j * AV_WIDTH + i) * 2];
                y = index[(j * AV_WIDTH + i) * 2 + 1];

                if (x == 0xff && y == 0xff) {
                    if (fread(pixmap->data, 8 * 8, 1, avfile) != 1) {
                        fprintf(stderr,
                                "lord: corrupted av file %s.av: can't read block data\n",
                                name);
                        exit(1);
                    }

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
        lord_poll_events();

        //      while( !lord_kb_hit() ) lord_poll_events(); lord_get_key();

        if (!read_av_audio_sample(audiobuf, &audiobufpos, avfile))
            break;

        if (fread(index, 2 * AV_WIDTH * AV_HEIGHT, 1, avfile) != 1)
            break;

        av_frame_tmp = av_frame_old;
        av_frame_old = av_frame;
        av_frame = av_frame_tmp;

        av_frame_num++;

    }

    while (playing_sample() && !lord_key_esc()) {
        Timer(10);
        lord_poll_events();
    }

    stop_sample(audiobuf);

    free(audiobuf);

    pixmap_free(pixmap);
    pixmap_free(av_frame);
    pixmap_free(av_frame_old);

    lord_reset_keyboard();

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
