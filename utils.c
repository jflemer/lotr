/****************************************************************************

    utils.c
    various utilities
    (memory management, file manipulation, etc... )


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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>



char lord_filename[1024];


/*
  allocates memory, exits on error
*/
void *
lord_malloc(int size)
{
    void *result;
    result = malloc(size);

    if (result == NULL) {
        char message[128];
        sprintf(message, "lord: can not allocate memory %d bytes of memory",
                size);
        perror(message);
        exit(1);
    }

    return result;

}




/*
  return full path to $HOME/.lord/<name>
*/

extern char *
lord_homedir_filename(const char *name)
{

    char cwd[1024];
    char *home = getenv("HOME");

    if (home == NULL) {
        fprintf(stderr,
                "lord: HOME variable not set (can not find users homedir)\n");
        exit(1);
    }


    getcwd(cwd, 1024);
    sprintf(lord_filename, "%s/.lord/", home);
    if (chdir(lord_filename)) {
        mkdir(lord_filename, S_IRWXU);
        if (chdir(lord_filename)) {
            perror("can not create directory $HOME/.lord/");
            exit(1);
        }
    }

    chdir(cwd);

    sprintf(lord_filename, "%s/.lord/%s", home, name);

    return lord_filename;

}



/*
  opens a file, exits on error
*/

FILE *
lord_fopen(const char *path, const char *mode)
{
    FILE *result;

    lord_homedir_filename(path);

    result = fopen(lord_filename, mode);

    if (result != NULL)
        return result;

    if (mode[0] == 'w') {
        perror("can not write to directory $HOME/.lord/");
        exit(1);
    }



    sprintf(lord_filename, "%s/%s", DATA_DIRECTORY, path);

    result = fopen(lord_filename, mode);

    if (result == NULL) {
        char message[128];
        sprintf(message, "lord: can not open file %s", lord_filename);
        perror(message);
        exit(1);
    }

    return result;
}



/*
   check if file exists
*/

int
fileexists(const char *path)
{
    FILE *testfile;

    lord_homedir_filename(path);

    testfile = fopen(lord_filename, "rb");

    if (testfile != NULL) {
        fclose(testfile);
        return 1;
    }

    sprintf(lord_filename, "%s/%s", DATA_DIRECTORY, path);

    testfile = fopen(lord_filename, "rb");
    if (testfile != NULL) {
        fclose(testfile);
        return 1;
    }



    return 0;
}



/*
  adds suffix to a filename
*/

char *
addsuffix(const char *name, const char *suffix)
{
    int len;
    char *result;

    len = strlen(name) + strlen(suffix) + 2;

    result = lord_malloc(len);

    sprintf(result, "%s.%s", name, suffix);

    return result;

}






/*
  returns file length, -1 if file==NULL
*/

long
filelen(FILE *file)
{
    long currpos;               /* current position */
    long result;

    if (file == NULL)
        return -1;
    currpos = ftell(file);

    fseek(file, 0, SEEK_END);

    result = ftell(file);

    fseek(file, currpos, SEEK_SET);

    return result;

}


/*
  random number
*/

int
lord_rnd(int n)
{
    int k;
    k = ((double)n) * rand() / (RAND_MAX + 1.0);
    if (k < 0)
        k = 0;
    ++k;
    if (k > n)
        k = n;
    return k;
}



/*
  add integer property node to a given xml node
*/

void
lord_save_prop_int(xmlNodePtr node, char *name, int value)
{
    char buf[16];

    sprintf(buf, "%d", value);

    xmlNewTextChild(node, NULL, (const xmlChar *)name, (const xmlChar *)buf);
}


/*
  add field property node to a given xml node
*/

void
lord_save_prop_field(xmlNodePtr node, char *name, Uint8 *value, int len)
{
    int i, d, e;
    char *buf;

    buf = lord_malloc(2 * len + 1);

    for (i = 0; i < len; ++i) {
        d = value[i];
        e = d / 0x10;
        if (e < 10)
            buf[2 * i] = e + '0';
        else
            buf[2 * i] = e - 10 + 'a';

        e = d % 0x10;
        if (e < 10)
            buf[2 * i + 1] = e + '0';
        else
            buf[2 * i + 1] = e - 10 + 'a';
    }

    buf[2 * len] = 0;

    xmlNewTextChild(node, NULL, (const xmlChar *)name, (const xmlChar *)buf);

    free(buf);

}



/*
  gets node with a given name
 */

xmlNodePtr
lord_get_subnode(xmlNodePtr node, const xmlChar *name, int force)
{
    xmlNodePtr cur = node->xmlChildrenNode;
    while (cur != NULL) {
        if (!xmlStrcmp(cur->name, name))
            break;
        cur = cur->next;
    }

    if (force && cur == NULL) {
        fprintf(stderr, "lord: can not get node named %s\n", name);
        exit(1);
    }

    return cur;

}


/*
  read integer property from node
*/

int
lord_load_prop_int(xmlNodePtr node, char *name)
{
    xmlNodePtr cur;
    cur = lord_get_subnode(node, (const xmlChar *)name, 1);

    return atoi((char *)xmlNodeGetContent(cur));
}


/*
  read integer property from node, return default if no such node
*/

int
lord_load_prop_int_default(xmlNodePtr node, char *name, int default_value)
{
    xmlNodePtr cur;
    cur = lord_get_subnode(node, (const xmlChar *)name, 0);

    if (cur == NULL)
        return default_value;

    return atoi((char *)xmlNodeGetContent(cur));
}


/*
  read field property node from a given xml node
*/

int
lord_load_prop_field(xmlNodePtr node, char *name, Uint8 *value, int maxlen)
{
    xmlChar *buf;
    int len;
    int i;
    int d, r;

    xmlNodePtr cur;
    cur = lord_get_subnode(node, (const xmlChar *)name, 1);

    buf = xmlNodeGetContent(cur);
    len = xmlStrlen(buf) / 2;
    if (len > maxlen)
        len = maxlen;

    for (i = 0; i < len; ++i) {
        r = 0;
        d = buf[2 * i];
        if (d >= '0' && d <= '9')
            r = d - '0';
        if (d >= 'a' && d <= 'f')
            r = d - 'a' + 10;

        r = r * 0x10;

        d = buf[2 * i + 1];
        if (d >= '0' && d <= '9')
            r += d - '0';
        if (d >= 'a' && d <= 'f')
            r += d - 'a' + 10;

        value[i] = r;

    }

    return len;
}




/*
  return data directory
*/
char *
lord_data_directory(void)
{
    return DATA_DIRECTORY;
}
