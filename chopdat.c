
/* chopdat.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chopdat.h"
#include "dvector/dvctor.h"

static chopdat_csv_table *record(chopdat_csv_table *t, chopdat_buffer *b);
static chopdat_csv_table *enclosed_token(chopdat_csv_table *t, chopdat_buffer *b);
static chopdat_csv_table *token(chopdat_csv_table *t, chopdat_buffer *b);
static chopdat_csv_table *crlf(chopdat_csv_table *t, chopdat_buffer *b);
static chopdat_csv_table *dquote(chopdat_csv_table *t, chopdat_buffer *b);

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
    chopdat_csv_perror error;
};

struct chopdat_buffer
{
    char *content;
    size_t content_size;
};

char *chopdat_slurp_file(const char *file_path, size_t *size)
{
    char *buffer = NULL;
    FILE *f = fopen(file_path, "r");

    if (!f) goto error;
    if (fseek(f, 0L, SEEK_END) < 0) goto error;

    long m = ftell(f);

    if (m < 0) goto error;
    if (fseek(f, 0L, SEEK_SET) < 0) goto error;

    buffer = malloc(m * sizeof(char) + 1);

    if (!buffer) goto error;

    size_t n = fread(buffer, 1, m, f);
    buffer[m] = '\0';

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

chopdat_buffer *chopdat_buffer_create(const char *file_path)
{
    chopdat_buffer *b = malloc(sizeof(*b));

    if (!b) return NULL;

    size_t *content_size = malloc(sizeof(*content_size));

    if (!content_size) return NULL;

    char *content = chopdat_slurp_file(file_path, content_size);

    if (!content) return NULL;

    b->content = content;
    b->content_size = *content_size;

    return b;
}

void chopdat_buffer_destroy(chopdat_buffer *b)
{
    if (!b) return;
    free(b->content);
    free(b);
}

char *chopdat_buffer_content(chopdat_buffer *b)
{
    if (!b) return NULL;
    return b->content;
}

int chopdat_buffer_size(chopdat_buffer *b)
{
    if (!b) return -1;
    return b->content_size;
}

chopdat_token *chopdat_token_create(char *s)
{
    chopdat_token *t = malloc(sizeof(*t));

    if (!t) return NULL;

    t->start = s;
    t->length = 0;

    return t;
}

void chopdat_token_destroy(chopdat_token *t)
{
    if (!t) return;
    free(t);
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

chopdat_csv_table *chopdat_csv_table_create(void)
{
    chopdat_csv_table *t = malloc(sizeof(*t));

    if (!t) return NULL;

    t->tokens = dvector_create();

    if (!t->tokens) return NULL;

    t->rows = 0;
    t->columns = 0;
    t->error = CHOPDAT_CSV_NOERR;

    return t;
}

void chopdat_csv_table_destroy(chopdat_csv_table *t)
{
    if (!t) return;

    dvector *v = t->tokens;
    int m = dvector_length(v);

    for (int i = 0; i < m; i++) {
        chopdat_token_destroy(dvector_get_element(v, i));
    }

    dvector_destroy(v);
    free(t);
}

size_t chopdat_csv_table_nrows(chopdat_csv_table *t)
{
    if (!t) return 0;
    return t->rows;
}

size_t chopdat_csv_table_ncols(chopdat_csv_table *t)
{
    if (!t) return 0;
    return t->columns;
}

chopdat_csv_perror chopdat_csv_table_errcode(chopdat_csv_table *t)
{
    if (!t) return CHOPDAT_CSV_NOTABLE;
    return t->error;
}

chopdat_token *chopdat_csv_table_spit_token(chopdat_csv_table *t, size_t l)
{
    if (!t) return NULL;
    dvector *v = t->tokens;
    return dvector_get_element(v, l);
}

chopdat_csv_table *chopdat_csv_file(chopdat_buffer *b)
{
    chopdat_csv_table *t = chopdat_csv_table_create();

    if (!b) {
        t->error = CHOPDAT_CSV_NULLBUF;
        return t;
    }

    char *l = b->content;
    size_t nr = 0;

    while (*(b->content) != '\0' && t->error == CHOPDAT_CSV_NOERR) {
        record(t, b);
        nr = nr + 1;
    }

    b->content = l;
    t->rows = nr;
    t->columns = t->columns / t->rows + 1;
    return t;
}

static chopdat_csv_table *record(chopdat_csv_table *t, chopdat_buffer *b)
{
    t = enclosed_token(t, b);

    if (t->error != CHOPDAT_CSV_NOERR) return t;

    if (*(b->content) == ',') {
        size_t s = b->content_size;
        size_t v = 0;

        while (v < s && *(b->content) == ',') {
            t->columns = t->columns + 1;
            b->content = b->content + 1;
            t = enclosed_token(t, b);
            if (t->error != CHOPDAT_CSV_NOERR) return t;
        }
    }

    return crlf(t, b);
}

static chopdat_csv_table *enclosed_token(chopdat_csv_table *t, chopdat_buffer *b) {
    t = dquote(t, b);

    if (t->error != CHOPDAT_CSV_NOERR) return t;

    t = token(t, b);

    if (t->error != CHOPDAT_CSV_NOERR) return t;

    return dquote(t, b);
}


static chopdat_csv_table *token(chopdat_csv_table *t, chopdat_buffer *b) {
    size_t s = b->content_size;
    size_t v = 0;

    if (*(b->content) == '\n' || *(b->content) == '"') {
        t->error = CHOPDAT_CSV_NOERR;
        return t;
    }

    chopdat_token *k = chopdat_token_create(b->content);

    while (v < s && (*(b->content) != '\n' && *(b->content) != '"'))
    {
        b->content = b->content + 1;
        v++;
    }

    if (*(b->content) != '"') {
        t->error = CHOPDAT_CSV_SYNTAX;
        chopdat_token_destroy(k);
        return t;
    }

    k->length = v;
    dvector_append(t->tokens, k);

    return t;
}

static chopdat_csv_table *crlf(chopdat_csv_table *t, chopdat_buffer *b) {
    if (*(b->content) != '\n') {
        t->error = CHOPDAT_CSV_SYNTAX;
        return t;
    }

    b->content = b->content + 1;
    return t;
}

static chopdat_csv_table *dquote(chopdat_csv_table *t, chopdat_buffer *b) {
    if (*(b->content) != '"') {
        t->error = CHOPDAT_CSV_SYNTAX;
        return t;
    }

    b->content = b->content + 1;
    return t;
}

/* chopdat.c ends here*/
