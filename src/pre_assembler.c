/**
 * pre_assembler.c
 * Responsible for executing the pre-assembler stage of the assembler,
 * which mainly includes macro expansion, comment lines and white lines removal and basic error detection.
 * Operates on a given .as and produces, if execution succeeds, a corresponding .am file.
 */

#include "pre_assembler.h"
#include "globals.h"
#include "handle_file.h"
#include "macros.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

/* Comment line prefix character */
#define COMMENT_LINE_CHAR ';'


/**
 * Checks if a given line is a comment line.
 * A comment line should begin with a COMMENT_LINE_CHAR after optional padding white characters.
 * @param line Input line
 * @param result Pointer to the result value: 1 if given line is a comment line, 0 if it's not
 * @return OK on success.
 *          ERR_NULL_PTR on NULL pointer input,
 *          in which case value pointed by result should be ignored.
 */
return_val is_comment_line(const char *line, int *result);

/**
 * Pre-assembler helper function.
 * Copies a line into the .am file and if it's a macro reference, expands the macro into .am instead.
 * Behavior is undefined if given file pointer doesn't point to an open file with write permissions,
 * or if given macro table is invalid.
 * @param line_buffer Line to copy (or expand) to .am file
 * @param am_fp .am file pointer
 * @param macro_table_p Pointer to macro table
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          ERR_CHARS_AFTER_MACRO_RED if non-white characters appear after macro reference line.
 *          ERR_FAILED_TO_WRITE_AM if .am file writing failed.
 */
return_val pre_assembler_handle_am_file_write(const char *line_buffer, FILE *am_fp, const macro_table *macro_table_p);

return_val pre_assembler_loop(const char *assembly_file_name, macro_table *macro_table_p) {
    return_val rv; /* Used for general return_val values */
    int result;    /* Used for general integer result values */

    int found_error;      /* Indicates if an error has already occurred */
    char *curr_file_name; /* Holds the .as file name for printing errors */
    FILE *as_fp, *am_fp;  /* File descriptors for old .as file and new .am file */

    char line_buffer[MAX_LINE_SIZE + 2]; /* Additional 2 bytes for '\n' and '\0' characters */
    unsigned int line_count;             /* Used for tracking current line count*/

    int macro_flag;        /* Flag that signals if we're inside a macro definition */
    char *curr_macro_name; /* Holds the currently defined macro name */

    if (!assembly_file_name || !macro_table_p) {
        handle_error(ERR_NULL_PTR, NULL, 0);
    }

    /* Set curr_file_name to the name of the .as file */
    curr_file_name = malloc_helper(strlen(assembly_file_name) + strlen(AS_FILE_EXTENSION) + 1);
    strcpy(curr_file_name, assembly_file_name);
    strcat(curr_file_name, AS_FILE_EXTENSION);

    /* Open .as file for reading */
    rv = fopen_helper(assembly_file_name, AS_FILE_EXTENSION, "r", &as_fp);
    /* If an error occurred when opening the .as file, give additional info */
    if (rv == ERR_FAILED_TO_OPEN_FILE) {
        handle_error(ERR_FAILED_TO_OPEN_AS_FOR_READ, NULL, 0);
    }
    else if (rv != OK) {
        handle_error(rv, NULL, 0);
    }

    /* Open .am file for writing */
    rv = fopen_helper(assembly_file_name, AM_FILE_EXTENSION, "w", &am_fp);
    /* If an error occurred when opening the .am file, give additional info */
    if (rv == ERR_FAILED_TO_OPEN_FILE) {
        handle_error(ERR_FAILED_TO_OPEN_AM_FOR_WRITE, NULL, 0);
    }
    else if (rv != OK) {
        handle_error(rv, NULL, 0);
    }


    /* Initialize macro table */
    rv = init_macro_table(macro_table_p);
    if (rv != OK)
        handle_error(rv, NULL, 0);


    /* Initialize flags and line count */
    found_error = 0;
    macro_flag = 0;
    line_count = 0;
    while ((rv = get_line(line_buffer, MAX_LINE_SIZE, as_fp)) != REACHED_EOF) {
        line_count++;
        if (rv != OK) {
            handle_error(rv, curr_file_name, line_count);
            found_error = 1;
            continue;
        }

        /* Initialize helper variables to avoid misuse */
        curr_macro_name = NULL;

        /* If line is empty, skip it */
        if (is_empty_line(line_buffer))
            continue;

        /* If line is a comment, skip it */
        rv = is_comment_line(line_buffer, &result);
        if (rv != OK) {
            handle_error(rv, curr_file_name, line_count);
            found_error = 1;
            continue;
        }
        else if (result == 1)
            continue;


        /* Check if line is a macro definition start */
        rv = is_macro_start(line_buffer, &curr_macro_name, macro_table_p);

        /* If an error occurred */
        if (rv != OK) {
            handle_error(rv, curr_file_name, line_count);
            found_error = 1;
            continue;
        }
        /* Otherwise, if line is a macro definition start (signaled by output macro name's value),
         * create corresponding macro entry and add it to macro table. */
        else if (curr_macro_name) {
            /* Add macro entry (with empty content and macro_name name) to macro table */
            rv = add_macro_to_table(curr_macro_name, NULL, macro_table_p);
            if (rv != OK)
                handle_error(rv, NULL, 0);

            /* Turn on macro flag */
            macro_flag = 1;

            /* Handling was successful. Read next line */
            continue;
        }

        /* If a macro definition hasn't been finished yet */
        else if (macro_flag == 1) {
            /* If the macro ending keyword is encountered, turn off macro flag */
            rv = is_macro_end(line_buffer, &result);
            if (rv != OK) {
                handle_error(rv, curr_file_name, line_count);
                found_error = 1;
                continue;
            }
            else if (result == 1) {
                macro_flag = 0;
                continue;
            }

            /* Otherwise, add the current line into the current macro's content */
            else {
                /* Note that it is assumed that no nested macro definitions exist,
                 * and therefore the last inserted macro entry to table points to the currently defined macro entry */
                rv = add_macro_content(macro_table_p->macro_entries[macro_table_p->macro_count - 1], line_buffer);
                if (rv != OK) {
                    handle_error(rv, NULL, 0);
                }

                /* Line content was successfully added to macro entry's content. Go to next line */
                continue;
            }
        }


        /* Otherwise, current line should be simply copied to .am file or expanded in the case of a macro reference */
        rv = pre_assembler_handle_am_file_write(line_buffer, am_fp, macro_table_p);
        if (rv != OK) {
            handle_error(rv, curr_file_name, line_count);
            found_error = 1;
            continue;
        }

        /* Go to next line */
        continue;
    }


    /* Free previously allocated memory and close files */

    if (fclose(as_fp)) {
        handle_error(ERR_FAILED_TO_CLOSE_AS, NULL, 0);
    }
    if (fclose(am_fp)) {
        handle_error(ERR_FAILED_TO_CLOSE_AM, NULL, 0);
    }

    free(curr_file_name);

    /* If an error occurred during the stage, delete the .am file, free the macro table, and report fail */
    if (found_error == 1) {
        rv = remove_helper(assembly_file_name, AM_FILE_EXTENSION);
        if (rv != OK)
            handle_error(ERR_FAILED_TO_REMOVE_AM, NULL, 0);

        rv = clear_macro_table(macro_table_p);
        if (rv != OK)
            handle_error(rv, NULL, 0);

        return PRE_ASSEMBLER_FAIL;
    }

    return PRE_ASSEMBLER_SUCCESS;
}

return_val is_comment_line(const char *line, int *result) {
    return_val rv;
    char *token_string; /* Used for storing a non-white character sequence of line */

    if (!line || !result)
        return ERR_NULL_PTR;

    /* Initialize result to avoid function misuse of the caller */
    *result = 0;

    /* Get first token string of line */
    rv = get_nth_token(line, 1, &token_string);
    if (rv != OK)
        return rv;

        /* If a first token exists, and it begins with COMMENT_LINE_CHAR then it's a comment line */
    else if (token_string != NULL && token_string[0] == COMMENT_LINE_CHAR)
        *result = 1;

        /* Otherwise, it's not a comment line */
    else
        *result = 0;

    free(token_string);
    return OK;
}

return_val pre_assembler_handle_am_file_write(const char *line_buffer, FILE *am_fp, const macro_table *macro_table_p) {
    return_val rv;
    macro_entry *expanded_macro_entry;
    size_t write_size, written_size;

    if (!line_buffer || !macro_table_p || !am_fp)
        return ERR_NULL_PTR;

    /* If line is a macro reference */
    rv = is_valid_macro_reference(line_buffer, macro_table_p, &expanded_macro_entry);
    if (rv != OK) {
        return rv;
    }
    /* If current line is a macro reference line, expand it into the .am file */
    else if (expanded_macro_entry != NULL) {
        /* If expanded macro has empty content, expansion is skipped */
        if (expanded_macro_entry->macro_content == NULL)
            return OK;

        write_size = strlen(expanded_macro_entry->macro_content);
        written_size = fwrite(expanded_macro_entry->macro_content, sizeof(char), write_size, am_fp);
        /* If write failed */
        if (written_size < write_size) {
            return ERR_FAILED_TO_WRITE_AM;
        }

        return OK;
    }

    /* Otherwise, copy current line into the .am file */
    else {
        write_size = strlen(line_buffer);
        written_size = fwrite(line_buffer, sizeof(char), write_size, am_fp);
        /* If write failed */
        if (written_size < write_size) {
            return ERR_FAILED_TO_WRITE_AM;
        }

        return OK;
    }
}