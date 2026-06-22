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
 *          ERR_ALLOC_FAILED on memory allocation fail and ERR_NULL_PTR on NULL pointer input,
 *          in which case value pointed by result should be ignored.
 */
return_val is_comment_line(const char *line, int *result);

int is_macro_start(const char *line);



return_val pre_assembler_loop(const char *assembly_file_name, macro_table *macro_table_p) {
    int found_error;                          /* Indicates if an error has already occurred */
    int macro_flag;                           /* Flag that signals if we're inside a macro definition */
    char *curr_file_name;                     /* Holds the .as file name for printing errors */
    unsigned int line_count;                  /* Used for tracking current line count*/
    FILE *old_fp, *new_fp;                    /* File descriptors for old .as file and new .am file */
    int result;                               /* Used for general integer result values */
    return_val rv;                            /* Used for general return_val values */
    char line_buffer[MAX_LINE_SIZE + 2];      /* Additional 2 bytes for '\n' and '\0' characters */

    if (!macro_table_p) {
        print_error(ERR_NULL_PTR, NULL, NULL, 0);
        return ERR_PRE_ASSEMBLER_FAIL;
    }

    /* Set curr_file_name to the name of the .as file */
    curr_file_name = (char *) malloc(strlen(assembly_file_name) + strlen(".as") + 1);
    if (!curr_file_name) {
        print_error(ERR_ALLOC_FAILED, NULL, NULL, 0);
        return ERR_PRE_ASSEMBLER_FAIL;
    }
    strcpy(curr_file_name, assembly_file_name);
    strcat(curr_file_name, ".as");

    rv = fopen_helper(assembly_file_name, ".as", "r", &old_fp);
    /* If an error occurred when opening the .as file, give additional info */
    if (rv == ERR_FAILED_TO_OPEN_FILE) {
        print_error(rv, "failed to read .as file", NULL, 0);
        free(curr_file_name);
        return ERR_PRE_ASSEMBLER_FAIL;
    }
    else if (rv != OK) {
        print_error(rv, NULL, NULL, 0);
        free(curr_file_name);
        return ERR_PRE_ASSEMBLER_FAIL;
    }

    rv = fopen_helper(assembly_file_name, ".am", "w", &new_fp);
    /* If an error occurred when opening the .am file, give additional info */
    if (rv == ERR_FAILED_TO_OPEN_FILE) {
        fclose(old_fp);
        print_error(rv, "failed to write to the .am file", NULL, 0);
        free(curr_file_name);
        return ERR_PRE_ASSEMBLER_FAIL;
    }
    else if (rv != OK) {
        fclose(old_fp);
        print_error(rv, NULL, NULL, 0);
        free(curr_file_name);
        return ERR_PRE_ASSEMBLER_FAIL;
    }


    /* Initialize flags and line count */
    found_error = 0;
    macro_flag = 0;
    line_count = 0;
    while (1) {
        line_count++;

        rv = get_line(line_buffer, MAX_LINE_SIZE, old_fp);
        /* If no bytes were left to read, exit loop */
        if (rv == ERR_ONLY_EOF)
            break;
        else if (rv != OK) {
            print_error(rv, NULL, curr_file_name, line_count);
            found_error = 1;
            continue;
        }

        /* If line is empty, skip it */
        if (is_empty_line(line_buffer))
            continue;

        /* If line is a comment, skip it */
        rv = is_comment_line(line_buffer, &result);
        if (rv != OK) {
            print_error(rv, NULL, curr_file_name, line_count);
            found_error = 1;
            continue;
        }
        else if (result == 1)
            continue;


        /* TODO: implement */
        /* If a macro definition hasn't been finished yet */
        if (macro_flag == 1) {
            /* If the macro ending keyword is encountered */

        }

        /* TODO: implement */
        /* If a macro definition start keyword is encountered */
        else if (is_macro_start(line_buffer)) {
            /* Check if macro's name is valid (not the same as a saved keyword) */
        }

        /* TODO: implement */
        /* Otherwise, if no error has been encountered yet, current line should be simply copied */
        else if (!found_error){

        }


        /*TODO:
         * check if mcro, mcroend, comment, empty
         */

    }

    /* Free previously allocated memory and close files */

    fclose(old_fp);
    fclose(new_fp);

    /* If an error occurred during the stage, delete the .am file, free the macro table, and report fail */
    if (found_error) {
        rv = remove_helper(assembly_file_name, ".am");
        if (rv != OK)
            print_error(rv, "failed to remove the .am file", curr_file_name, 0);

        free(curr_file_name);
        return ERR_PRE_ASSEMBLER_FAIL;
    }

    free(curr_file_name);
    return OK;
}

return_val is_comment_line(const char *line, int *result) {
    return_val rv;
    char *token_string; /* Used for storing a non-white character sequence of line */

    /* Initialize result value to -1 to avoid function misuse of the caller */
    *result = -1;

    if (!line)
        return ERR_NULL_PTR;

    /* Get first token string of line */
    rv = get_nth_token(line, 1, &token_string);
    if (rv != OK)
        return rv;

    /* If there is no token in the line (it's empty) */
    if (!token_string)
        *result = 0;

    /* If a first token exists, and it begins with COMMENT_LINE_CHAR then it's a comment line */
    else if (token_string[0] == COMMENT_LINE_CHAR)
        *result = 1;

    /* Otherwise, a first token exists, but it's not a comment line */
    else
        *result = 0;

    free(token_string);
    return OK;
}


 return OK;
}



}

int is_macro_start(const char *line) {
    /* TODO: Make sure valid line doesn't have trailing non-white characters */
}

int is_macro_end(const char *line) {
    /* TODO: Make sure valid line doesn't have trailing non-white characters */
}

int is_valid_macro_name(const char *name) {
    /* TODO: Think of a way to implement saved keyword list */
}
