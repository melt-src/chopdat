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
    size_t *content_size = malloc(sizeof(size_t));
    char *content = chopdat_slurp_file(file_path, content_size);

    if (!content) {
        fprintf(stderr, "error: could not read file into memory.\n");
        exit(1);
    }

    printf("%ldB\n%s\n", *content_size, content);

    /* printing a token */
    chopdat_token *t = chopdat_token_create(&content[1],5);
    chopdat_token_spit(t);

    return 0;
}
