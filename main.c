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
    chopdat_buffer *b1 = chopdat_buffer_create(file_path);

    if (!b1) {
        fprintf(stderr, "error: could not read file into memory.\n");
        exit(1);
    }

    size_t s1 = chopdat_buffer_size(b1);
    char *c1 = chopdat_buffer_content(b1);

    printf("\n%ldB\n%s\n", s1, c1);

    /* =====| PARSING SIMULATION |===== */

    /* lexing simulation: manually creatining tokens */
    chopdat_token *x = chopdat_token_create(&c1[1], 5);
    chopdat_token *y = chopdat_token_create(&c1[15], 4);

    /* parsing simulation: creating a csv table and manually adding tokens */
    chopdat_csv_table *t1 = chopdat_csv_table_create();
    chopdat_csv_table_eattok(t1, x);
    chopdat_csv_table_eattok(t1, y);
    chopdat_csv_table_spittok(t1);

    /* freeing memory */
    chopdat_csv_table_destroy(t1);
    chopdat_buffer_destroy(b1);

    /* =====| ACTUAL PARSING |===== */

    /* due to the previous buffer being deallocated, the file need to be read
    into memory again */
    chopdat_buffer *b2 = chopdat_buffer_create(file_path);
    chopdat_buffer *nullbuf = NULL;
    size_t s2 = chopdat_buffer_size(b2);
    char *c2 = chopdat_buffer_content(b2);

    chopdat_csv_table *t2 = chopdat_csv(nullbuf);

    if (chopdat_csv_table_errcode(t2) == CHOPDAT_CSV_NULLBUF) {
        fprintf(stderr, "error: no data to parse\n");
    }

    return 0;
}
