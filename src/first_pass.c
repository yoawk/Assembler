/**
 * first_pass.c
 * Handles the first pass stage: partial code image and data image encoding,
 * label handling, and .string, .data, .extern directive handling.
 */

#include "first_pass.h"

#include "globals.h"
#include "handle_file.h"
#include "instructions.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>


return_val first_pass_loop(const char *assembly_file_name, macro_table *macro_table_p, symbol_table *symbol_table_p,
                           ) {
    return_val rv;
    unsigned int i;
    int result;          /* General use helper function's result */
    char *helper_string; /* General use string */

    int found_error; /* Found error flag */

    FILE *am_fp; /* .am file's file pointer */
    char *curr_file_name;
    char line_buffer[MAX_LINE_SIZE + 2]; /* Additional 2 characters for '\n' and '\0' */
    unsigned int line_count;
    unsigned short IC, DC;  /* Instruction Counter and Data Counter */





    if (!assembly_file_name || !macro_table_p) {
        handle_error(ERR_NULL_PTR, NULL, 0);
    }


    /* Set curr_file_name to the name of the .am file */
    curr_file_name = (char *) malloc_helper(strlen(assembly_file_name) + strlen(AM_FILE_EXTENSION) + 1);
    strcpy(curr_file_name, assembly_file_name);
    strcat(curr_file_name, AM_FILE_EXTENSION);

    /* Open .am file for reading */
    rv = fopen_helper(assembly_file_name, AM_FILE_EXTENSION, "r", &am_fp);
    /* If an error occurred when opening the .am file, give additional info */
    if (rv == ERR_FAILED_TO_OPEN_FILE) {
        handle_error(ERR_FAILED_TO_OPEN_AM_FOR_READ, NULL, 0);
    }
    else if (rv != OK) {
        handle_error(rv, NULL, 0);
    }
    /* Close .am file and free allocated memory */
    if (fclose(am_fp)) {
        handle_error(ERR_FAILED_TO_CLOSE_AM, NULL, 0);
        found_error = 1;
    }
    free(curr_file_name);


    return FIRST_PASS_SUCCESS;
}

