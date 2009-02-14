
#include<stdio.h>
#include<string.h>

#define BUFLEN 64000

int
main(int argc, char *argv[])
{

    char datname[20];
    char dataname[30];
    char ndxname[20];

    FILE *dat, *ndx, *data;

    int l, n, p, pp;

    unsigned char ndxbuf[4];
    char buf[BUFLEN];

    if (argc != 2) {
        printf("Usage: dendx <datfile>\n");
        return 1;
    }

    strncpy(datname, argv[1], 19);
    datname[19] = 0;
    strncpy(ndxname, argv[1], 19);
    ndxname[19] = 0;

    l = strlen(ndxname);
    if (l < 3)
        return 1;
    ndxname[l - 3] = 'n';
    ndxname[l - 2] = 'd';
    ndxname[l - 1] = 'x';

    if ((dat = fopen(datname, "rb")) == NULL) {
        printf("Can not open %s\n", datname);
        return 1;
    }

    if ((ndx = fopen(ndxname, "rb")) == NULL) {
        printf("Can not open %s\n", ndxname);
        return 1;
    }

    n = 0;
    pp = 0;
    while (fread(ndxbuf, 1, 4, ndx) == 4) {
        p = 0;
        p = ((ndxbuf[3] * 256 + ndxbuf[2]) * 256 + ndxbuf[1]) * 256 +
            ndxbuf[0];
        if ((!(p == 0 && n == 0)) && p - pp < BUFLEN) {
            sprintf(dataname, "%s.%03d", datname, n);
            data = fopen(dataname, "wb");
            fseek(dat, pp, SEEK_SET);
            fread(buf, p - pp, 1, dat);
            fwrite(buf, p - pp, 1, data);
            fclose(data);
            pp = p;
            ++n;
        }
    }

    p = fread(buf, 1, BUFLEN, dat);

    if (p) {
        sprintf(dataname, "%s.%03d", datname, n);
        data = fopen(dataname, "wb");
        fwrite(buf, 1, p, data);
        fclose(data);
    }

    return 0;

}
