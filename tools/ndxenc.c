#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[]) {
    char filename[1024];
    int size = 0;
    int i;
    FILE *ndxout;
    FILE *datout;

    if (argc < 3) {
        printf("Usage: ndxdec <datfile> [input file]...\n");
        return 1;
    }

    snprintf(filename, sizeof(filename), "%s.dat", argv[1]);
    if ((datout = fopen(filename, "wb")) == NULL) {
        perror(filename);
        exit(1);
    }

    snprintf(filename, sizeof(filename), "%s.ndx", argv[1]);
    if ((ndxout = fopen(filename, "wb")) == NULL) {
        perror(filename);
        exit(1);
    }

    for (i = 2; i < argc; ++i) {
        char indata[256 * 1024];
        int insize;
        char outdata[512 * 1024];
        int outsize = 0;
        FILE *in;

        if ((in = fopen(argv[i], "rb")) == NULL) {
            perror(argv[i]);
            exit(1);
        }

        insize = fread(indata, 1, sizeof(indata), in);
        fclose(in);

        if (insize > 0) {
            int outp = 0;
            int flagspos = 0;
            int curflag = 0x100;
            int inp = 0;
            while (inp < insize) {
                // This is safe
                // int cur_pos = (inp / 0x1000) * 0x1000 + 0x12;
                // This should work too
                int cur_pos = inp - 0xfff;
                if (cur_pos < 0)
                    cur_pos = 0;
                int copy_pos = 0;
                int copy_size = 0;
                if (curflag > 0x80) {
                    curflag = 1;
                    flagspos = outp;
                    outdata[outp++] = 0xff;
                }

                while (cur_pos < inp) {
                    int i;
                    for (i = 0; i <= 18 && inp + i < insize; ++i) {
                        if (indata[cur_pos + i] != indata[inp + i])
                            break;
                    }

                    if (i > copy_size) {
                        copy_pos = cur_pos;
                        copy_size = i;
                    }
                    cur_pos++;
                }

                if (copy_size < 3) {
                    outdata[outp++] = indata[inp++];
                } else {
                    outdata[flagspos] -= curflag;
                    if (copy_size > 18)
                        copy_size = 18;
                    outdata[outp++] = ((copy_pos + 0x1000 - 0x12) % 0x1000) & 0xff;
                    outdata[outp] = (((copy_pos + 0x1000 - 0x12) % 0x1000) & 0xf00) >> 4;
                    outdata[outp++] |= copy_size - 3;
                    inp += copy_size;
                }

                curflag = curflag << 1;
            }

            outsize = outp;
        }

        if (fwrite(outdata, 1, outsize, datout) != outsize) {
            perror("dat");
            exit(1);
        }

        fputc(size & 0xff, ndxout);
        fputc((size >> 8) & 0xff, ndxout);
        fputc((size >> 16) & 0xff, ndxout);
        if (fputc((size >> 24) & 0xff, ndxout) == EOF) {
            perror("ndx");
            exit(1);
        }
        size += outsize;
    }

    fclose(datout);
    fclose(ndxout);

    return 0;
}
