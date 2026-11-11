#include <stdio.h>
#include <stdlib.h>
#include "chopdat.h"

#define MIN_NARGS 2

int main(int argc, char **argv)
{
    /* arguments parsing */
    if (argc < MIN_NARGS) {
        fprintf(stderr, "error: input file not provided");
        exit(1);
    }

    const char *file_path = argv[1];

    /* reading a file into memory */
    chopdat_buffer *buffer = chopdat_buffer_create(file_path);

    if (!buffer) {
        fprintf(stderr, "error: could not read file into memory.\n");
        exit(1);
    }

    size_t s = chopdat_buffer_size(buffer);
    char *c = chopdat_buffer_content(buffer);

    printf("\n%ldB\n%s\n", s, c);

    /* creatining a token */
    chopdat_token *a = chopdat_token_create(&c[1],5);
    chopdat_token *b = chopdat_token_create(&c[15],4);

    /* creating a csv table and adding tokens */
    chopdat_csv_table *table = chopdat_csv_table_create();
    chopdat_csv_table_eattok(table, a);
    chopdat_csv_table_eattok(table, b);
    chopdat_csv_table_spittok(table);

    /* freeing memory */
    chopdat_csv_table_destroy(table);
    chopdat_buffer_destroy(buffer);

    return 0;
}
