/**
 * handle_file.c
 * Utilities for file handling and line handling
 */

#include "handle_file.h"

#include "utils.h"

#include <stdlib.h>
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
        return REACHED_EOF;

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

    concat_name = (char *) malloc_helper(strlen(file_name) + strlen(file_extension) + 1);
    strcpy(concat_name, file_name);

    strcat(concat_name, file_extension);

    *fp = fopen(concat_name, mode);
    free(concat_name);
    if (!(*fp))
        return ERR_FAILED_TO_OPEN_FILE;

    return OK;
}

return_val remove_helper(const char *file_name, const char *file_extension) {
    char *concat_name;

    if (!file_name || !file_extension)
        return ERR_NULL_PTR;

    /* Concatenate file_name and file_extension into concat_name */
    concat_name = (char *) malloc_helper(strlen(file_name) + strlen(file_extension) + 1);
    strcpy(concat_name, file_name);
    strcat(concat_name, file_extension);

    /* Remove file with error checking */

    if (remove(concat_name) != 0) {
        free(concat_name);
        return ERR_FAILED_TO_REMOVE_FILE;
    }

    free(concat_name);
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

return_val get_nth_token(const char *line, int n, char **token) {
    int token_count;
    int token_len;
    const char *p, *start;  /* p is used for string iteration and start indicates start of token */

    if (!line || !token)
        return ERR_NULL_PTR;

    p = line;
    token_count = 1;

    while (*p) {
        /* Skip white characters */
        while (*p && (*p == ' ' || *p == '\t' || *p == '\n'))
            p++;

        /* There are no non-white characters left and we haven't reached the nth token */
        if (!*p)
            break;

        /* Start of token */
        start = p;

        /* Move to end of token */
        while (*p && *p != ' ' && *p != '\t' && *p != '\n')
            p++;

        if (token_count == n) {
            token_len = p - start + 1;       /* Size of token including '\0' */
            *token = (char *) malloc_helper(token_len);

            /* Copy nth token string into token and add null character */
            memcpy(*token, start, token_len - 1);
            (*token)[token_len - 1] = '\0';

            return OK;
        }

        token_count++;
    }

    /* If there is no nth token */
    *token = NULL;
    return OK;
}



return_val count_parameters(const char *line, unsigned int *output_count) {
    unsigned int i;  /* Used for indexing*/
    char c; /* Used for storing characters of string */

    /*
     * 0: Haven't reached name yet
     * 1: Reading name
     * 2: Reading space characters after name
     * 3: After previous states
     */
    int name_flag;

    /*
     * -1: Before first parameter
     * 0: Reading a parameter
     * 1: Not reading a parameter and haven't found a comma
     * 2: Not reading a parameter and found a comma
     */
    int comma_flag;

    if (!line || !output_count)
        return ERR_NULL_PTR;
    *output_count = 0;


    comma_flag = -1;
    name_flag = 0;

    /* Go over line and check comma validity
     * Update comma and command name flags accordingly and detect errors.
     */
    for (i = 0; i < strlen(line); i++) {
        c = line[i];

        /* If c is a white character */
        if (c == ' ' || c == '\t' || c == '\n') {
            if (name_flag == 1)
                name_flag = 2;
            else if (comma_flag == 0)
                comma_flag = 1;
            continue;
        }

        /* If c is a comma */
        else if (c == ',') {
            if (name_flag != 3)
                return ERR_EARLY_COMMA;
            if (comma_flag == 2)
                return ERR_MULTIPLE_COMMAS;

            comma_flag = 2;
            continue;
        }


        /* Otherwise, c is some other character */
        else {
            /* If this is first character of name */
            if (name_flag == 0)
                name_flag = 1;
            /* If this is first character of first parameter */
            else if (name_flag == 2) {
                (*output_count)++;
                name_flag = 3;
                comma_flag = 0;
            }
            /* If this is first character of parameter but no comma separated it from another parameter */
            else if (comma_flag == 1 && name_flag == 3)
                return ERR_MISSING_COMMA;
            /* If this is first character of a parameter and a comma separated it from previous parameter */
            else if (comma_flag == 2 && name_flag == 3) {
                (*output_count)++;
                comma_flag = 0;
            }

            continue;
        }
    }

    /* If no name found */
    if (name_flag == 0)
        return ERR_NO_NAME;

    /* If a comma appeared after the last parameter */
    if (comma_flag == 2)
        return ERR_EXTRA_COMMA;

    return OK;
}
