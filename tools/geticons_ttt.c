
/* 

get item data from towers.exe

 */


#include<stdio.h>


int start[4] = { 0x0b, 0x0b, 0x09, 0x09 };

int
main(void)
{

    FILE *in;
    int n;

    in = fopen("towers.exe", "rb");

    n = 0;

    /* find start sequence  */

    while (n < 4) {
        if (fgetc(in) == start[n])
            ++n;
        else
            n = 0;
    }

    printf("int icon_width[160] = { 0x0b, 0x0b, 0x09, 0x09");

    for (n = 4; n < 160; ++n) {
        printf(", 0x%02x", fgetc(in));
    }

    printf("};\n");

    fclose(in);

    return 0;
}
