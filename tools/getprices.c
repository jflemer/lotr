
/* 

get characters data from lord.exe

 */


#include<stdio.h>


int start[8] = { 01, 00, 01, 00, 01, 00, 01, 00 };

int
main(void)
{

    FILE *in;
    int n, p;

    in = fopen("lord.exe", "rb");

    n = 0;

    /* find start sequence  */

    while (n < 8) {
        if (fgetc(in) == start[n])
            ++n;
        else
            n = 0;
    }



    printf("int object_prices[] = { ");

    n = 0;

    while (n < 0x100 - 0x60) {

        if (n < 4) {
            p = 1;
        }
        else {
            p = fgetc(in);
            p += 0x100 * fgetc(in);
        }

        printf("/* %2x */ %d,\n                        ", n + 0x60, p);

        ++n;

    }

    printf("-1 };\n");

    return 0;
}
