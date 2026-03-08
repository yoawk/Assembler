/**
 * handle_file.c
 * Utilities for file handling and line handling
 */

#include "../include/handle_file.h"
#include "../include/utils.h"

#include <string.h>

/**
 * Cleans up characters until '\n' is reached or EOF is reached
 * @param fp File descriptor
 */
void cleanup_line(FILE *fp);

return_val get_line(char *output_line, size_t max_size, FILE *fp) {
    if (!output_line || !fp)
        return ERR_NULL_PTR;

    if (fgets(output_line, max_size + 2, fp)) {
        /* If a new line character was found within max_size + 1 characters */
        if (strchr(output_line, '\n') != NULL)
            return OK;

        /* If no new line character was found within max_size + 1 characters
         * but EOF has been reached and output_line size is no more than max_size characters */
        if (feof(fp) && strlen(output_line) <= max_size)
            return OK;

        /* Otherwise, line is too big */
        cleanup_line(fp);
        return ERR_LINE_TOO_BIG;
    }


    /* No bytes were read and EOF has been reached */
    else if (feof(fp))
        return ERR_EOF;

    /* New line character was not found within max_size + 1 characters */
    else {
        cleanup_line(fp);
        return ERR_LINE_TOO_BIG;
    }
}

return_val fopen_helper(const char *file_name, const char *file_extension, const char *mode, FILE **fp) {
    char *concat_name;

    if (!file_name || !file_extension || !mode)
        return ERR_NULL_PTR;

    concat_name = (char *) malloc(strlen(file_name) + strlen(file_extension) + 1);
    if (!concat_name)
        return ERR_ALLOC_FAILED;
    strcpy(concat_name, file_name);

    strcat(concat_name, file_extension);

    *fp = fopen(concat_name, mode);
    free(concat_name);
    if (!*fp)
        return ERR_FAILED_TO_OPEN_FILE;

    return OK;
}

int is_empty_line(const char *line) {
    int i;
    for (i = 0; line[i] != '\0'; i++) {
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
            return 0;
    }

    return 1;
}


void cleanup_line(FILE *fp) {
    while (getc(fp) != '\n' && !feof(fp));
}