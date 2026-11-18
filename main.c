#include <stdio.h>
#include <stdlib.h>
#include "chopdat.h"

#define MIN_NARGS 2

/* TEST
 *
 * This file only purpose is to test library features. */
int main(int argc, char **argv)
{
    /* arguments parsing */
    if (argc < MIN_NARGS) {
        fprintf(stderr, "error: input file not provided");
        exit(1);
    }

    const char *file_path = argv[1];

    /* reading a file into memory */
    chopdat_buffer *b = chopdat_buffer_create(file_path);

    if (!b) {
        fprintf(stderr, "error: could not read file into memory.\n");
        exit(1);
    }

    size_t s = chopdat_buffer_size(b);
    char *c = chopdat_buffer_content(b);

    printf("\n%ldB\n%s\n", s, c);

    /* =====| PARSING |===== */
    chopdat_csv_table *t = chopdat_csv_file(b);
    chopdat_csv_perror e = chopdat_csv_table_errcode(t);
    int nr = chopdat_csv_table_nrows(t);
    int nc = chopdat_csv_table_ncols(t);

    switch (e) {
    case -1:
        fprintf(stderr, "error: the table is null\n");
        break;
    case 0:
        printf("success: the file %s is well formatted\n", file_path);
        break;
    case 1:
        fprintf(stderr, "error: no data to parse\n");
        break;
    case 2:
        fprintf(stderr, "%s:%d:%d error: invalid syntax\n", file_path, nr, nc);
        break;
    default:
        fprintf(stderr, "bollito\n");
        break;

    }

    chopdat_csv_table_destroy(t);

    return 0;
}
