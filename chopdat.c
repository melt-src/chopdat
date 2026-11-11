/* chopdat.c */

#include "chopdat.h"
#include "dvector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct chopdat_token
{
    char *start;
    size_t length;
};

struct chopdat_csv_table
{
    dvector *tokens;
    size_t rows;
    size_t columns;
};

chopdat_token *chopdat_token_create(char *s, size_t l)
{
    chopdat_token *t = malloc(sizeof(*t));

    if (!t) return NULL;

    t->start = s;
    t->length = l;

    return t;
}

void chopdat_token_destroy(chopdat_token *t)
{
    if (!t) return;

    free(t->start);
    free(t);

    return;
}

char *chopdat_token_start(chopdat_token *t)
{
    if (!t) return NULL;
    return t->start;
}

size_t chopdat_token_length(chopdat_token *t)
{
    if (!t) return 0;
    return t->length;
}

void chopdat_token_spit(chopdat_token *t)
{
    if (!t || !t->start) return;

    fwrite(t->start, 1, t->length, stdout);
}

char *chopdat_slurp_file(const char *file_path, size_t *size)
{
    char *buffer = NULL;
    FILE *f = fopen(file_path, "r");

    if (!f) goto error;
    if (fseek(f, 0L, SEEK_END) < 0) goto error;

    long m = ftell(f);

    if (m < 0) goto error;
    if (fseek(f, 0L, SEEK_SET) < 0) goto error;

    buffer = malloc(m * sizeof(char));

    if (!buffer) goto error;

    size_t n = fread(buffer, 1, m, f);

    if (ferror(f)) goto error;
    if (n != (size_t) m) goto error;
    if (size) *size = n;

    fclose(f);

    return buffer;

error:
    if (f) fclose(f);
    if (buffer) free(buffer);
    return NULL;
}

/* chopdat.c ends here*/
