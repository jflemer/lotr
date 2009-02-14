/****************************************************************************
 
    midi.h
    midi music support
    inspiration taken from exult


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
#include "midi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


int playmidi_pid = -1;
int midifile = -1;
char miditmpname[] = "/tmp/lordXXXXXX";

/*
  stops music return nonzero if music was not playing
 */

int
StopMidi(void)
{

    int unlink_result;

    unlink_result = unlink(miditmpname);

    if (unlink_result == 0) {
        kill(playmidi_pid, SIGTERM);
        /* this should be the playmidi process */
        kill(playmidi_pid + 1, SIGTERM);
        usleep(250000);
        kill(playmidi_pid, SIGKILL);
        kill(playmidi_pid + 1, SIGKILL);
    }

    return unlink_result;

}


/*
  start music
*/

void
PlayMidi(Uint8 * data, int size)
{

    char shellcommand[256];

    if (data == NULL || size == 0) {
#ifdef CD_VERSION
        return;
#endif

#ifndef TTT
        fprintf(stderr, "lord: trying to play an empty midi file\n");
        exit(1);
#else
        return;
#endif
    }

    StopMidi();

    miditmpname[9] = miditmpname[10] = miditmpname[11] = miditmpname[12] =
        miditmpname[13] = miditmpname[14] = 'X';
    if ((midifile = mkstemp(miditmpname)) < 0
        || write(midifile, data, size) != size) {
        fprintf(stderr, "lord: can not create temporary midifile\n");
        exit(1);
    }

    close(midifile);

    playmidi_pid = fork();
    if (!playmidi_pid) {
        sprintf(shellcommand, "%s %s 1>/dev/null; rm -f %s",
                PLAYMIDI_COMMAND, miditmpname, miditmpname);
        execlp("/bin/sh", "/bin/sh", "-c", shellcommand, NULL);
        raise(SIGKILL);
    }

}
