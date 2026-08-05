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


/* .ent and .ext file field sizes */
#define ENTRIES_FILE_FIELD_LOCATION_SIZE 4
#define EXTERNS_FILE_FIELD_LOCATION_SIZE 4


/**
 * Second pass helper function.
 * Goes over .entry lines in the .am file and updates symbols' entry attribute accordingly.
 * Behavior is undefined if input is invalid.
 * Handles all errors internally.
 * @param assembly_file_name Assembly file name with no extension
 * @param am_file_name Name of the .am file
 * @param macro_table_p Pointer to macro table
 * @param symbol_table_p Pointer to symbol table
 * @param output_found_error Pointer to the found error flag. Will be updated accordingly.
 * @return void
 */
void second_pass_update_entry_symbols(const char *assembly_file_name, const char *am_file_name, const macro_table *macro_table_p,
                                      symbol_table *symbol_table_p, int *output_found_error);

/**
 * Produces the .ent file.
 * Will not produce an empty .ent file.
 * Behavior is undefined if input is invalid.
 * @param assembly_file_name Input assembly file name.
 * @param symbol_table_p Pointer to symbol table.
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          ERR_FAILED_TO_OPEN_ENT_FOR_WRITE if .ent file opening for write failed.
 *          ERR_FAILED_TO_WRITE_ENT if writing operation to .ent file failed.
 *          ERR_FAILED_TO_CLOSE_ENT if closing of .ent file failed.
 *          ERR_FAILED_TO_REMOVE_ENT if removal of .ent file failed.
 */
return_val produce_ent_file(const char *assembly_file_name, symbol_table *symbol_table_p);

/**
 * Produces the .ext file.
 * Will not produce an empty .ext file.
 * Behavior is undefined if inputs is invalid.
 * @param assembly_file_name Input assembly file name
 * @param code_image Code image array
 * @param ICF Final IC value
 * @param symbol_table_p Pointer to symbol table.
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          ERR_FAILED_TO_OPEN_EXT_FOR_WRITE if .ext file opening for write failed.
 *          ERR_FAILED_TO_WRITE_EXT if writing operation to .ext file failed.
 *          ERR_FAILED_TO_CLOSE_EXT if closing of .ext file failed.
 *          ERR_FAILED_TO_REMOVE_EXT if removal of .ext file failed.
 */
return_val produce_ext_file(const char *assembly_file_name, const code_entry code_image[], unsigned short ICF,
                            const symbol_table *symbol_table_p);

/**
 * Checks if a given line contains a label definition.
 * Assumes first pass had already checked label validity.
 * @param line Input line
 * @param output_label_flag Pointer to output label flag. 1 if line contains label, 0 otherwise.
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 */
return_val contains_label(const char *line, int *output_label_flag);

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

    /* Find .entry lines in the .am file and update entry symbol attributes */
    second_pass_update_entry_symbols(assembly_file_name, curr_file_name, macro_table_p, symbol_table_p, &found_error);


    /* If no errors were found, produce the output files */
    if (found_error != 1) {
        /* Note that produced errors in this segment will be program errors */

        /* Produce .ob file */
        rv = produce_object_file(assembly_file_name, code_image, data_image, ICF, DCF, symbol_table_p);
        if (rv != OK) {
            handle_error(rv, NULL, 0);
        }

        /* Produce .ent file */
        rv = produce_ent_file(assembly_file_name, symbol_table_p);
        if (rv != OK)
            handle_error(rv, NULL, 0);

        /* Produce .ext file */
        rv = produce_ext_file(assembly_file_name, code_image, ICF, symbol_table_p);
        if (rv != OK)
            handle_error(rv, NULL, 0);
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


void second_pass_update_entry_symbols(const char *assembly_file_name, const char *am_file_name, const macro_table *macro_table_p,
                                      symbol_table *symbol_table_p, int *output_found_error) {
    return_val rv;
    int result;

    int label_flag;

    unsigned int line_count;
    char line_buffer[MAX_LINE_SIZE + 2];
    symbol_entry *temp_symbol_entry;
    directive_entry temp_directive_entry;
    FILE *am_fp;

    if (!assembly_file_name || !am_file_name || !macro_table_p || !symbol_table_p || !output_found_error)
        handle_error(ERR_NULL_PTR, NULL, 0);

    /* Open .am file for reading */
    rv = fopen_helper(assembly_file_name, AM_FILE_EXTENSION, "r", &am_fp);
    /* If an error occurred when opening the .am file, give additional info */
    if (rv == ERR_FAILED_TO_OPEN_FILE) {
        handle_error(ERR_FAILED_TO_OPEN_AM_FOR_READ, NULL, 0);
    }
    else if (rv != OK) {
        handle_error(rv, NULL, 0);
    }


    /* Go over .am file and handle .entry directives */
    line_count = 0;
    while ((rv = get_line(line_buffer, MAX_LINE_SIZE, am_fp)) != REACHED_EOF) {
        line_count++;
        if (rv != OK) {
            handle_error(rv, am_file_name, line_count);
            *output_found_error = 1;
            continue;
        }

        label_flag = 0;

        /* Initialize helper variables to avoid miuse*/
        temp_symbol_entry = NULL;


        /* Check if current line contains a label definition */
        rv = contains_label(line_buffer, &label_flag);
        if (rv != OK) {
            handle_error(rv, am_file_name, line_count);
            *output_found_error = 1;
            continue;
        }


        /* Check if line is a directive line */
        rv = is_valid_directive_line(line_buffer, label_flag, &temp_directive_entry, &result);
        if (rv != OK) {
            handle_error(rv, am_file_name, line_count);
            *output_found_error = 1;
            continue;
        }
        /* If line is a directive line (in which case the directive entry is set accordingly) */
        else if (result == 1) {
            if (strcmp(temp_directive_entry.directive_name, ".entry") == 0) {
                temp_symbol_entry = is_in_symbol_table(temp_directive_entry.directive_data.symbol_name, symbol_table_p);
                /* If symbol isn't present in symbol table */
                if (!temp_symbol_entry) {
                    handle_error(ERR_SYMBOL_DOES_NOT_EXIST, am_file_name, line_count);
                    *output_found_error = 1;
                    continue;
                }

                /* If symbol is external */
                else if ((temp_symbol_entry->attributes & SYMBOL_EXTERN_ATTR) != 0) {
                    handle_error(ERR_SYMBOL_ENTRY_IS_EXTERNAL, am_file_name, line_count);
                    *output_found_error = 1;
                    continue;
                }

                /* If symbol already has entry attribute, then current .entry can be skipped */
                else if ((temp_symbol_entry->attributes & SYMBOL_ENTRY_ATTR) != 0) {
                    continue;
                }

                /* Otherwise .entry's symbol is set as entry symbol */
                temp_symbol_entry->attributes |= SYMBOL_ENTRY_ATTR;

                /* .entry directive handled successfully */
                continue;
            }

            /* If directive is not .entry, skip line */
            else
                continue;
        }
        /* If line is not a directive line, skip it */
        continue;
    }


    if (fclose(am_fp) == EOF)
        handle_error(ERR_FAILED_TO_CLOSE_AM, NULL, 0);

    return;
}

return_val produce_ent_file(const char *assembly_file_name, symbol_table *symbol_table_p) {
    return_val rv;
    unsigned int i;

    symbol_entry *curr_symbol_entry; /* Current symbol entry in the symbol table loop */

    int is_file_empty = 1; /* 1 if the produced file is empty, 0 otherwise */
    FILE *ent_fp;          /* .ent file pointer */
    char *write_buffer;    /* File write buffer */
    size_t write_size, written_size;

    if (!assembly_file_name || !symbol_table_p)
        return ERR_NULL_PTR;

    /* Open .ent file for writing */
    rv = fopen_helper(assembly_file_name, ENT_FILE_EXTENSION, "w", &ent_fp);
    /* Add additional context if opening failed */
    if (rv == ERR_FAILED_TO_OPEN_FILE)
        return ERR_FAILED_TO_OPEN_ENT_FOR_WRITE;
    else if (rv != OK)
        return rv;

    /* Go over symbol table and search for symbol with entry attribute */
    for (i = 0; i < symbol_table_p->symbol_count; i++) {
        /* Symbol entries array should be set */
        if (!(symbol_table_p->symbol_entries))
            return ERR_NULL_PTR;

        /* The current symbol entry dealt with */
        curr_symbol_entry = (symbol_table_p->symbol_entries)[i];
        /* Assumed symbol validity */
        if (!curr_symbol_entry)
            return ERR_NULL_PTR;

        /* If symbol has entry attribute, add it to .ent file */
        if ((curr_symbol_entry->attributes & SYMBOL_ENTRY_ATTR) != 0) {
            /* If .ent file is empty, go down a line. Otherwise, update empty file flag */
            if (is_file_empty == 1) {
                is_file_empty = 0;
            }
            else {
                if (fputc('\n', ent_fp) == EOF)
                    return ERR_FAILED_TO_WRITE_ENT;
            }

            /* Add it to .ent file */

            /* the line format of the .ent file is:
             * (SYMBOL NAME)(SPACE)(4 DIGITS OF LOCATION IN DECIMAL WITH PADDING ZEROS)
             * Where the location is ENTRIES_FILE_FIELD_LOCATION_SIZE decimal digits with zero padding.
             */

            /* Calculate the appropriate write size according to the format */
            write_size = strlen(curr_symbol_entry->name) + 1 + ENTRIES_FILE_FIELD_LOCATION_SIZE;
            /* Setup string buffer */
            write_buffer = malloc_helper(write_size + 1); /* Added 1 byte for '\0' */
            sprintf(write_buffer, "%s %0*hu", curr_symbol_entry->name, ENTRIES_FILE_FIELD_LOCATION_SIZE, curr_symbol_entry->location);

            /* Perform write */
            written_size = fwrite(write_buffer, sizeof(char), write_size, ent_fp);
            free(write_buffer);
            /* If write failed */
            if (written_size < write_size)
                return ERR_FAILED_TO_WRITE_ENT;

            /* Current entry symbol was written successfully */
        }
    }

    /* Close .ent file */
    if (fclose(ent_fp) == EOF)
        return ERR_FAILED_TO_CLOSE_ENT;

    /* If no data was written, then delete the file */
    if (is_file_empty == 1) {
        rv = remove_helper(assembly_file_name, ENT_FILE_EXTENSION);
        if (rv == ERR_FAILED_TO_REMOVE_FILE)
            return ERR_FAILED_TO_REMOVE_ENT;
        else if (rv != OK)
            return rv;
    }

    /* .ent was produced successfully */
    return OK;
}


return_val produce_ext_file(const char *assembly_file_name, const code_entry code_image[], unsigned short ICF,
                            const symbol_table *symbol_table_p) {
    return_val rv;
    unsigned short i;
    symbol_entry *curr_symbol_entry;

    int is_file_empty = 1; /* 1 if the produced file is empty, 0 otherwise */
    FILE *ext_fp;
    char *write_buffer; /* Buffer for .ext file writing */
    size_t write_size, written_size;

    if (!assembly_file_name || !code_image || !symbol_table_p)
        return ERR_NULL_PTR;

    /* Open .ext file for writing */
    rv = fopen_helper(assembly_file_name, EXT_FILE_EXTENSION, "w", &ext_fp);
    if (rv == ERR_FAILED_TO_OPEN_FILE) {
        return ERR_FAILED_TO_OPEN_EXT_FOR_WRITE;
    }
    else if (rv != OK)
        return rv;

    /* Search for code entries that depend on an external symbol and add them to .ext file */
    for (i = INITIAL_IC; i < ICF; i++) {
        /* The only operand addressing mode that can depend on an external symbol is direct */
        if (code_image[i].operand_addr_mode == DIRECT_MODE) {
            curr_symbol_entry = is_in_symbol_table(code_image[i].symbol_name, symbol_table_p);

            /* Symbol should exist since input code image is assumed to be valid */
            if (!curr_symbol_entry)
                return ERR_NULL_PTR;

            /* If operand's associated symbol is external, write to .ext file */
            if ((curr_symbol_entry->attributes & SYMBOL_EXTERN_ATTR) != 0) {
                /* Add it to .ext file */

                /* If .ext file is empty, go down a line. Otherwise, update empty file flag */
                if (is_file_empty == 1) {
                    is_file_empty = 0;
                }
                else {
                    if (fputc('\n', ext_fp) == EOF)
                        return ERR_FAILED_TO_WRITE_EXT;
                }

                /* the line format of the .ext file is:
                 * (SYMBOL NAME)(SPACE)(CODE ENTRY LOCATION)
                 * Where the code entry's location is EXTERN_FILE_FIELD_LOCATION_SIZE decimal digits with zero padding.
                 */

                /* Calculate the appropriate write size according to the format */
                write_size = strlen(curr_symbol_entry->name) + 1 + EXTERNS_FILE_FIELD_LOCATION_SIZE;
                /* Setup string buffer */
                write_buffer = malloc_helper(write_size + 1); /* Added 1 byte for '\0' */
                sprintf(write_buffer, "%s %0*hu", curr_symbol_entry->name, EXTERNS_FILE_FIELD_LOCATION_SIZE, i);

                /* Perform write */
                written_size = fwrite(write_buffer, sizeof(char), write_size, ext_fp);
                free(write_buffer);
                /* If write failed */
                if (written_size < write_size)
                    return ERR_FAILED_TO_WRITE_EXT;

                /* Written successfully */
            }
        }
    }

    /* Close file */
    if (fclose(ext_fp) == EOF)
        return ERR_FAILED_TO_CLOSE_EXT;


    /* If no data was written, then delete the file */
    if (is_file_empty == 1) {
        rv = remove_helper(assembly_file_name, EXT_FILE_EXTENSION);
        if (rv == ERR_FAILED_TO_REMOVE_FILE)
            return ERR_FAILED_TO_REMOVE_EXT;
        else if (rv != OK)
            return rv;
    }


    return OK;
}

return_val contains_label(const char *line, int *output_label_flag) {
    return_val rv;

    size_t string_size;
    char *helper_string;

    if (!line || !output_label_flag)
        return ERR_NULL_PTR;

    *output_label_flag = 0;

    /* Get first token */
    rv = get_nth_token(line, 1, &helper_string);
    if (rv != OK)
        return rv;

    /* Check if token is non-empty and ends with LABEL_END_CHAR (assumes first pass has checked label validity) */
    if (helper_string != NULL) {
        string_size = strlen(helper_string);
        if (string_size > 0 && helper_string[string_size - 1] == LABEL_END_CHAR)
            *output_label_flag = 1;
        free(helper_string);
    }

    return OK;
}
