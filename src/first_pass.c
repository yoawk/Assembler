/**
 * first_pass.c
 * Handles the first pass stage: partial code image and data image encoding,
 * label handling, and .string, .data, .extern directive handling.
 */

#include "first_pass.h"

#include "globals.h"
#include "handle_file.h"
#include "instructions.h"
#include "directives.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>


/**
 * First pass helper function.
 * Handles the encoding of an instruction and its (possible) operands in the code image.
 * Also handles inserting the symbol of a label that precedes the instruction into the symbol table.
 * Assumes inputs (that are needed for the operation) are valid.
 *
 * @param input_instruction_entry Instruction entry to encode
 * @param input_operand_src Source operand entry to encode (may not be needed)
 * @param input_operand_dst Destination operand entry to encode (may not be needed)
 * @param label_flag 1 if a label is present in the instruction's line, 0 otherwise.
 * @param label_symbol_name Symbol name of the label (if present)
 * @param symbol_table_p Pointer to symbol table
 * @param line_count Instruction's line count in the .am file.
 * @param code_image Code image array
 * @param IC_p Current IC (will be updated)
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          ERR_INVALID_SRC_ADDR_MODE if source operand addressing mode is invalid.
 *          ERR_INVALID_DEST_ADDR_MODE if destination operand addressing mode is invalid.
 */
return_val first_pass_handle_instruction(const instruction_entry *input_instruction_entry, const operand_entry *input_operand_src,
                                         const operand_entry *input_operand_dst, int label_flag,
                                         const char *label_symbol_name, symbol_table *symbol_table_p, unsigned int line_count,
                                         code_entry code_image[], unsigned short *IC_p);

/**
 * First pass helper function.
 * Handles given directive entry with validation checking.
 * .data/.string directives will be inserted accordingly to data image,
 * .extern directives will add the extern symbol to the symbol table,
 * .entry directives will be ignored and left for second pass.
 *
 * @param input_directive_entry Input directive entry
 * @param label_flag 1 if directive contains a label definition in its line, 0 otherwise.
 * @param label_symbol_name Symbol name of the label (if it exists)
 * @param symbol_table_p Pointer to symbol table (might be altered on .data/.string/.extern directives).
 * @param macro_table_p Pointer to macro table
 * @param data_image Data image array
 * @param DC_p Pointer to the current DC.
 * @return OK on success.
 *         ERR_NULL_PTR on unexpected NULL.
 *         ERR_SYMBOL_EXTERN_ALREADY_DEFINED if .extern's symbol is already defined as non-external symbol.
 *         ERR_SYMBOL_IS_MACRO_NAME if a .extern's symbol is a previously defined macro name.
 */
return_val first_pass_handle_directive(directive_entry *input_directive_entry, int label_flag, const char *label_symbol_name,
                                       symbol_table *symbol_table_p, const macro_table *macro_table_p, data_entry data_image[],
                                       unsigned short *DC_p);

return_val first_pass_loop(const char *assembly_file_name, macro_table *macro_table_p, symbol_table *symbol_table_p,
                           code_entry code_image[], data_entry data_image[], unsigned short *ICF, unsigned short *DCF) {
    return_val rv;
    unsigned int i;
    int result;          /* General use helper function's result */
    char *helper_string; /* General use string */

    int found_error; /* Found error flag */
    int label_flag;  /* Found label definition flag */

    FILE *am_fp; /* .am file's file pointer */
    char *curr_file_name;
    char line_buffer[MAX_LINE_SIZE + 2]; /* Additional 2 characters for '\n' and '\0' */
    unsigned int line_count;
    unsigned short IC, DC;  /* Instruction Counter and Data Counter */

    /* Symbol related temporary variables */
    char label_symbol_name[MAX_SYMBOL_NAME_SIZE + 1];

    /* Instruction related temporary variables */
    const instruction_entry *temp_instruction_entry;
    operand_entry temp_operand_src, temp_operand_dst;

    /* Directive related temporary variables */
    directive_entry temp_directive_entry;


    if (!assembly_file_name || !macro_table_p || !symbol_table_p || !code_image || !data_image || !ICF || !DCF) {
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

    /* Initialize symbol table */
    rv = init_symbol_table(symbol_table_p);
    if (rv != OK) {
        handle_error(rv, NULL, 0);
    }

    /* Go over the input file lines */
    IC = INITIAL_IC;
    DC = INITIAL_DC;
    line_count = 0;
    found_error = 0;
    while ((rv = get_line(line_buffer, MAX_LINE_SIZE, am_fp)) != REACHED_EOF) {
        line_count++;
        if (rv != OK) {
            handle_error(rv, curr_file_name, line_count);
            found_error = 1;
            continue;
        }

        label_flag = 0; /* Discard the label flag status of the last line */

        /* Initialize helper variables to avoid misuse */
        helper_string = NULL;

        memset(&label_symbol_name, 0, sizeof(label_symbol_name));

        temp_instruction_entry = NULL;
        memset(&temp_operand_src, 0, sizeof(temp_operand_src));
        memset(&temp_operand_dst, 0, sizeof(temp_operand_dst));

        memset(&temp_directive_entry, 0, sizeof(directive_entry));


        /* Check if current line starts with a label/symbol definition */
        /* Helper string is used for the (possible) dynamic memory allocation of symbol name */
        rv = is_label_definition(line_buffer, symbol_table_p, macro_table_p, &helper_string);
        if (rv != OK) {
            handle_error(rv, curr_file_name, line_count);
            found_error = 1;
            continue;
        }
        /* Otherwise, if result is helper string isn't NULL, then line contains a valid label/symbol definition */
        /* Copy helper string's contents (symbol name) into the correct symbol name variable,
         * which might be used later. */
        else if (helper_string) {
            strcpy(label_symbol_name, helper_string);
            free(helper_string);
            label_flag = 1;
        }

        /* Check if line contains an instruction */
        rv = is_valid_instruction_line(line_buffer, label_flag, &temp_instruction_entry, &temp_operand_src, &temp_operand_dst);
        if (rv != OK) {
            handle_error(rv, curr_file_name, line_count);
            found_error = 1;
            continue;
        }
        /* If instruction entry isn't NULL, then line contains a valid instruction
         * (note that symbol resolution happens in second pass) */
        else if (temp_instruction_entry) {
            rv = first_pass_handle_instruction(temp_instruction_entry, &temp_operand_src, &temp_operand_dst, label_flag, label_symbol_name,
                                               symbol_table_p, line_count, code_image, &IC);
            if (rv != OK) {
                handle_error(rv, curr_file_name, line_count);
                found_error = 1;
                continue;
            }

            /* Encoding was successful. Go to next .am file line */
            continue;
        }

        /* Check if line is a directive line */
        rv = is_valid_directive_line(line_buffer, label_flag, &temp_directive_entry, &result);
        if (rv != OK) {
            handle_error(rv, curr_file_name, line_count);
            found_error = 1;
            continue;
        }
        /* If line is a directive line (in which case the directive entry is set accordingly) */
        else if (result == 1) {
            rv = first_pass_handle_directive(&temp_directive_entry, label_flag, label_symbol_name, symbol_table_p, macro_table_p,
                                             data_image, &DC);
            if (rv != OK) {
                handle_error(rv, curr_file_name, line_count);
                found_error = 1;
                continue;
            }

            continue;
        }


        /* If current line isn't an instruction or directive line, then it's invalid
         * (comment and empty lines were filtered in pre-assembler stage) */
        handle_error(ERR_UNKNOWN_LINE_TYPE, curr_file_name, line_count);
        found_error = 1;
        continue;
    }



    /* Close .am file and free allocated memory */
    if (fclose(am_fp)) {
        handle_error(ERR_FAILED_TO_CLOSE_AM, NULL, 0);
        found_error = 1;
    }
    free(curr_file_name);


    return FIRST_PASS_SUCCESS;
}

return_val first_pass_handle_instruction(const instruction_entry *input_instruction_entry, const operand_entry *input_operand_src,
                                         const operand_entry *input_operand_dst, int label_flag,
                                         const char *label_symbol_name, symbol_table *symbol_table_p, unsigned int line_count,
                                         code_entry code_image[], unsigned short *IC_p) {
    return_val rv;

    /* Note that not all pointers are necessarily non-NULL */
    if (!input_instruction_entry || !symbol_table_p || !code_image || !IC_p)
        return ERR_NULL_PTR;

    /* If line contains a label definition, add it to the symbol table */
    if (label_flag == 1) {
        /* Check if label's symbol name is not set */
        if (!label_symbol_name)
            return ERR_NULL_PTR;

        /* Add new symbol entry to symbol table with code attribute and IC location */
        rv = add_symbol_to_table(symbol_table_p, label_symbol_name, *IC_p, SYMBOL_CODE_ATTR);
        if (rv != OK) {
            return rv;
        }
    }

    /* Encode the instruction's first word and possibly some of its operand words */
    rv = encode_instruction(input_instruction_entry, input_operand_src, input_operand_dst, code_image, IC_p, line_count);
    if (rv != OK) {
        return rv;
    }

    /* Encoding was successful */
    return OK;
}

return_val first_pass_handle_directive(directive_entry *input_directive_entry, int label_flag, const char *label_symbol_name,
                                       symbol_table *symbol_table_p, const macro_table *macro_table_p, data_entry data_image[],
                                       unsigned short *DC_p) {
    return_val rv;

    symbol_entry *temp_symbol_entry;

    /* Note that label_symbol_name may be NULL and not used */
    if (!input_directive_entry || !symbol_table_p || !macro_table_p || !data_image || !DC_p)
        return ERR_NULL_PTR;

    /* If directive is .data */
    if (strcmp(input_directive_entry->directive_name, ".data") == 0) {
        /* If line contains a label definition, add it to the symbol table */
        if (label_flag == 1) {
            /* Check if label's symbol name is not set */
            if (!label_symbol_name)
                return ERR_NULL_PTR;

            /* Add new symbol entry to symbol table with data attribute and DC location */
            rv = add_symbol_to_table(symbol_table_p, label_symbol_name, *DC_p, SYMBOL_DATA_ATTR);
            if (rv != OK) {
                return rv;
            }

            /* If no errors occurred, then label was added to the symbol table successfully */
        }

        /* Encode the valid .data value list into data image */
        rv = encode_directive_data(input_directive_entry, data_image, DC_p);
        if (rv != OK) {
            return rv;
        }

        /* If no errors occurred, then .data directive's value list was successfully encoded */
        return OK;
    }

    /* If directive is .string */
    else if (strcmp(input_directive_entry->directive_name, ".string") == 0) {
        /* If line contains a label definition, add it to the symbol table */
        if (label_flag == 1) {
            /* Check if label's symbol name is not set */
            if (!label_symbol_name)
                return ERR_NULL_PTR;

            /* Add new symbol entry to symbol table with data attribute and DC location */
            rv = add_symbol_to_table(symbol_table_p, label_symbol_name, *DC_p, SYMBOL_DATA_ATTR);
            if (rv != OK) {
                return rv;
            }

            /* If no errors occurred, then label was added to the symbol table successfully */
        }

        /* Encode the valid .string's content into data image */
        rv = encode_directive_string(input_directive_entry, data_image, DC_p);
        if (rv != OK) {
            return rv;
        }

        /* If no errors occurred, then .string directive's content was successfully encoded */
        return OK;
    }

    /* If directive is .extern */
    else if (strcmp(input_directive_entry->directive_name, ".extern") == 0) {
        /* Note that if a label definition is present in current line, it'll be ignored */

        /* Check if a non-external symbol with the same name was already added to the symbol table */
        temp_symbol_entry = is_in_symbol_table((input_directive_entry->directive_data).symbol_name, symbol_table_p);
        if (temp_symbol_entry) {
            /* If previously added symbol entry isn't external, then report error */
            if ((temp_symbol_entry->attributes & SYMBOL_EXTERN_ATTR) == 0) {
                return ERR_SYMBOL_EXTERN_ALREADY_DEFINED;
            }

            /* Otherwise, current .extern symbol is already present in the symbol table with external attribute,
             * and current .extern can be ignored. */
            return OK;
        }
        /* Check if a previously defined macro's name collides with the .extern symbol name  */
        if (is_in_macro_table((input_directive_entry->directive_data).symbol_name, macro_table_p)) {
            return ERR_SYMBOL_IS_MACRO_NAME;
        }

        /* Otherwise, .extern symbol name is valid and the symbol isn't present in the symbol table yet */
        /* Add it to the symbol table with external attribute and SYMBOL_EXTERN_LOCATION location value */
        rv = add_symbol_to_table(symbol_table_p, (input_directive_entry->directive_data).symbol_name, SYMBOL_EXTERN_LOCATION,
                                 SYMBOL_EXTERN_ATTR);
        if (rv != OK) {
            return rv;
        }

        /* .extern symbol handling was successful */
        return OK;
    }
    /* Otherwise, directive is .entry, and it'll be handled in second pass */
    /* Note that if a label definition is present in current line, it'll be ignored */
    else
        return OK;
}
