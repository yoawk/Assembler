/**
 * second_pass.c
 * Handles the second pass stage of the assembler.
 * Fills empty code image encodings left by first pass,
 * handles .entry directives, and creates output .ent, .ext, .ob files.
 */

#include "second_pass.h"

#include "handle_file.h"
#include "globals.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>



return_val second_pass_loop(const char *assembly_file_name, macro_table *macro_table_p, symbol_table *symbol_table_p,
                            code_entry code_image[], data_entry data_image[], unsigned short ICF, unsigned short DCF) {
    return_val rv;
    unsigned short i;
    int found_error;

    char *curr_file_name;


    if (!assembly_file_name || !macro_table_p || !symbol_table_p || !code_image || !data_image) {
        handle_error(ERR_NULL_PTR, NULL, 0);
    }

    found_error = 0;

    /* Set curr_file_name to the name of the .am file */
    curr_file_name = (char *) malloc_helper(strlen(assembly_file_name) + strlen(AM_FILE_EXTENSION) + 1);
    strcpy(curr_file_name, assembly_file_name);
    strcat(curr_file_name, AM_FILE_EXTENSION);

    /* Update partial encodings in code image */
    for (i = INITIAL_IC; i < ICF; i++) {
        rv = update_encoding(code_image, i, symbol_table_p);
        if (rv != OK) {
            handle_error(rv, curr_file_name, code_image[i].am_file_location);
            found_error = 1;
        }
    }


    /* Free allocated memory */
    free(curr_file_name);

    /* Clear the symbol and macro tables */
    rv = clear_macro_table(macro_table_p);
    if (rv != OK)
        handle_error(rv, NULL, 0);

    rv = clear_symbol_table(symbol_table_p);
    if (rv != OK)
        handle_error(rv, NULL, 0);


    if (found_error == 1) {
        return SECOND_PASS_FAIL;
    }

    return SECOND_PASS_SUCCESS;
}


