#include "../archive.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[]) {
    Archive *archive;
    char dataname[1024];
    int i;

    if (argc != 2) {
        printf("Usage: ndxdec <datfile>\n");
        return 1;
    }

    archive = archive_ndx_open(argv[1]);

    for (i = 0; i < archive->size; ++i) {
        int size = 0;
        Uint8 *data = 0;
        FILE *out;

        if (archive_data_size(archive, i) > 0)
            data = decompress_ndxarchive(archive, i, &size);

        snprintf(dataname, sizeof(dataname), "%s.%03d", argv[1], i);
        if ((out = fopen(dataname, "wb")) == NULL) {
            perror(dataname);
            exit(1);
        }
        fwrite(data, 1, size, out);
        fclose(out);
        free(data);
    }

    archive_close(archive);

    return 0;
}
