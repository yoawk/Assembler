/**
 * pre_assembler.c
 * Responsible for macro expansion, filtering of comment lines and empty lines.
 *
 * Assumes there are no nested macro definitions
 * Assumes each macro definition start has a corresponding macro ending
 * Assumes macro calls appear after corresponding macro definitions
 *
 */

#include "../include/pre_assembler.h"
#include "../include/utils.h"
#include "../include/globals.h"
#include "../include/handle_file.h"

#define MACRO_START "mcro"
#define MACRO_END "mcroend"

/**
 * Checks if a given line is a comment line.
 * A comment line should begin with a ';' after optional padding white characters.
 * @param line Input line
 * @return 1 if line is a comment line, 0 otherwise
 */
int is_comment_line(const char *line);

int is_macro_start(const char *line);

int is_macro_end(const char *line);

int is_valid_macro_name(const char *name);

return_val pre_assembler_loop(const char *assembly_file_name) {
    int found_error;
    int macro_flag;
    unsigned int line;
    FILE *old_fp, *new_fp;
    return_val rv;
    char line_buffer[MAX_LINE_SIZE + 2];

    rv = fopen_helper(assembly_file_name, ".as", "r", &old_fp);
    if (rv != OK)
        return rv;

    rv = fopen_helper(assembly_file_name, ".am", "w", &new_fp);
    if (rv != OK) {
        fclose(old_fp);
        return rv;
    }

    found_error = 0;
    macro_flag = 0;
    line = 0;
    while (1) {
        if (feof(old_fp))
            break;

        line++;

        rv = get_line(line_buffer, MAX_LINE_SIZE, old_fp);
        if (rv != OK) {
            print_error(rv, line);
            found_error = 1;
            continue;
        }

        /* If line is empty, skip it */
        if (is_empty_line(line_buffer))
            continue;

        /* If line is a comment, skip it */
        else if (is_comment_line(line_buffer))
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

    fclose(old_fp);
    fclose(new_fp);


 return OK;
}


int is_comment_line(const char *line) {

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
