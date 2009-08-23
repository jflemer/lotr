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

#include "lotr.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#define HOME_DIR_STR "$HOME/.lotr/"

char lotr_filename[1024];


/*
  allocates memory, exits on error
*/
void *
lotr_malloc(int size)
{
    void *result;
    result = malloc(size);

    if (result == NULL) {
        char message[128];
        snprintf(message, sizeof(message),
                 "lotr: can not allocate %d bytes of memory", size);
        perror(message);
        exit(1);
    }

    return result;

}




/*
  return full path to $HOME/.lotr/<name>
*/

extern char *
lotr_homedir_filename(const char *name)
{
    char cwd[1024];
#if !defined(AMIGA_OS4) && !defined(WIN32)
    char *home = getenv("HOME");

    if (home == NULL) {
        fprintf(stderr,
                "lotr: HOME variable not set (can not find users homedir)\n");
        exit(1);
    }

    snprintf(lotr_filename, sizeof(lotr_filename), "%s/.lotr/", home);
#else
    snprintf(lotr_filename, sizeof(lotr_filename), "lord/");
#endif
    getcwd(cwd, 1024);
#ifndef WIN32 /* TODO : Do the check */
    if (chdir(lotr_filename)) { /* Somewhat stupid test for dir existence */
        if (mkdir(lotr_filename, S_IRWXU)) {
            perror("can not create directory " HOME_DIR_STR);
            exit(1);
        }
    }
#endif
    chdir(cwd);

#if !defined(AMIGA_OS4) && !defined(WIN32)
    snprintf(lotr_filename, sizeof(lotr_filename), "%s/.lotr/%s", home, name);
#else
    snprintf(lotr_filename, sizeof(lotr_filename), "lord/%s", name);
#endif

    return lotr_filename;

}



/*
  opens a file, exits on error
*/

FILE *
lotr_fopen(const char *path, const char *mode)
{
    FILE *result;

    lotr_homedir_filename(path);

    result = fopen(lotr_filename, mode);

    if (result != NULL)
        return result;

    if (mode[0] == 'w') {
        perror("can not write to directory " HOME_DIR_STR);
        exit(1);
    }

    snprintf(lotr_filename, sizeof(lotr_filename), "%s/%s", DATA_DIRECTORY,
             path);

    result = fopen(lotr_filename, mode);

    if (result == NULL) {
        char message[128];
        snprintf(message, sizeof(message), "lotr: can not open file %s", lotr_filename);
        perror(message);
        exit(1);
    }

    return result;
}



/*
   check if file exists
*/

int
lotr_file_exists(const char *path)
{
    FILE *testfile;

    lotr_homedir_filename(path);

    testfile = fopen(lotr_filename, "rb");

    if (testfile != NULL) {
        fclose(testfile);
        return 1;
    }

    snprintf(lotr_filename, sizeof(lotr_filename), "%s/%s", DATA_DIRECTORY,
             path);

    testfile = fopen(lotr_filename, "rb");
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
lotr_add_suffix(const char *name, const char *suffix)
{
    int len;
    char *result;

    len = strlen(name) + strlen(suffix) + 2;

    result = lotr_malloc(len);

    snprintf(result, len, "%s.%s", name, suffix);

    return result;

}




/*
  returns file length, -1 if file==NULL
*/

long
lotr_filelen(FILE *file)
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
lotr_rnd(int n)
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
lotr_save_prop_int(xmlNodePtr node, char *name, int value)
{
    char buf[16];

    snprintf(buf, sizeof(buf), "%d", value);

    xmlNewTextChild(node, NULL, (const xmlChar *)name, (const xmlChar *)buf);
}


/*
  add field property node to a given xml node
*/

void
lotr_save_prop_field(xmlNodePtr node, char *name, Uint8 *value, int len)
{
    int i, d, e;
    char *buf;

    buf = lotr_malloc(2 * len + 1);

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
lotr_get_subnode(xmlNodePtr node, const xmlChar *name, int force)
{
    xmlNodePtr cur = node->xmlChildrenNode;
    while (cur != NULL) {
        if (!xmlStrcmp(cur->name, name))
            break;
        cur = cur->next;
    }

    if (force && cur == NULL) {
        fprintf(stderr, "lotr: can not get node named %s\n", name);
        exit(1);
    }

    return cur;

}


/*
  read integer property from node
*/

int
lotr_load_prop_int(xmlNodePtr node, char *name)
{
    xmlNodePtr cur;
    cur = lotr_get_subnode(node, (const xmlChar *)name, 1);

    return atoi((char *)xmlNodeGetContent(cur));
}


/*
  read integer property from node, return default if no such node
*/

int
lotr_load_prop_int_default(xmlNodePtr node, char *name, int default_value)
{
    xmlNodePtr cur;
    cur = lotr_get_subnode(node, (const xmlChar *)name, 0);

    if (cur == NULL)
        return default_value;

    return atoi((char *)xmlNodeGetContent(cur));
}


/*
  read field property node from a given xml node
*/

int
lotr_load_prop_field(xmlNodePtr node, char *name, Uint8 *value, int maxlen)
{
    xmlChar *buf;
    int len;
    int i;
    int d, r;

    xmlNodePtr cur;
    cur = lotr_get_subnode(node, (const xmlChar *)name, 1);

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
lotr_data_directory(void)
{
    return DATA_DIRECTORY;
}
