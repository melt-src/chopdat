/* ============================ | --------------- | ============================
 *
 * ChopDat!
 *
 * This header file provides the prototipes of structured data and
 * functions implementing basic text parsing. See doc files for more.
 *
 * ============================ | --------------- | ========================== */

#ifndef CHOPDAT_H
#define CHOPDAT_H

#include <stddef.h>

typedef struct chopdat_csv_table chopdat_csv_table;
typedef struct chopdat_token chopdat_token;
typedef struct chopdat_buffer chopdat_buffer;

typedef enum chopdat_csv_perror
{
    CHOPDAT_CSV_NOERR = 0,
    CHOPDAT_CSV_SYNTAX
} chopdat_csv_perror;

char *chopdat_slurp_file(const char *, size_t *);

chopdat_buffer *chopdat_buffer_create(const char *);
void chopdat_buffer_destroy(chopdat_buffer *);
char *chopdat_buffer_content(chopdat_buffer *);
int chopdat_buffer_size(chopdat_buffer *);

chopdat_token *chopdat_token_create(char *, size_t);
void chopdat_token_destroy(chopdat_token *);
char *chopdat_token_start(chopdat_token *);
size_t chopdat_token_length(chopdat_token *);

chopdat_csv_table *chopdat_csv_table_create(void);
void chopdat_csv_table_destroy(chopdat_csv_table *);
size_t chopdat_csv_table_nrows(chopdat_csv_table *);
size_t chopdat_csv_table_ncols(chopdat_csv_table *);
int chopdat_csv_table_eattok(chopdat_csv_table *, chopdat_token *);
void chopdat_csv_table_spittok(chopdat_csv_table *);

chopdat_csv_table *chopdat_csv(const char *);

#endif /* CHOPDAT_H */

/* chopdat.h ends here */
