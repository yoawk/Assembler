/**
 * instructions.c
 * Responsible for validation checks and retrievals of the assembly instructions
 */

#include "instructions.h"

#include "handle_file.h"
#include "keywords.h"
#include "globals.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>



const instruction_entry *get_instruction(const char *inst_name) {
    static const instruction_entry instructions_table[] = {
        {"mov", 0, 0, 2, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE, DIRECT_MODE | REGISTER_MODE},
        {"cmp", 1, 0, 2, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE},
        {"add", 2, 10, 2, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE, DIRECT_MODE | REGISTER_MODE},
        {"sub", 2, 11, 2, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE, DIRECT_MODE | REGISTER_MODE},
        {"lea", 4, 0, 2, DIRECT_MODE, DIRECT_MODE | REGISTER_MODE},
        {"clr", 5, 10, 1, 0, DIRECT_MODE | REGISTER_MODE},
        {"not", 5, 11, 1, 0, DIRECT_MODE | REGISTER_MODE},
        {"inc", 5, 12, 1, 0, DIRECT_MODE | REGISTER_MODE},
        {"dec", 5, 13, 1, 0, DIRECT_MODE | REGISTER_MODE},
        {"jmp", 9, 10, 1, 0, DIRECT_MODE | RELATIVE_MODE},
        {"bne", 9, 11, 1, 0, DIRECT_MODE | RELATIVE_MODE},
        {"jsr", 9, 12, 1, 0, DIRECT_MODE | RELATIVE_MODE},
        {"red", 12, 0, 1, 0, DIRECT_MODE | REGISTER_MODE},
        {"prn", 13, 0, 1, 0, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE},
        {"rts", 14, 0, 0, 0, 0},
        {"stop", 15, 0, 0, 0, 0}
    };

    static const size_t instructions_count = sizeof(instructions_table) / sizeof(instructions_table[0]);
    size_t i;

    if (!inst_name)
        return NULL;

    /* Go over the instructions and check for a matching instruction name */
    for (i = 0; i < instructions_count; i++)
        if (strcmp(instructions_table[i].name, inst_name) == 0)
            return &(instructions_table[i]);

    /* If inst_name didn't match any instruction name */
    return NULL;
}

int is_instruction_keyword(const char *input_string) {
    if (!input_string)
        return 0;

    if (get_instruction(input_string) != NULL)
        return 1;
    return 0;
}


return_val get_operand(const char *operand_str, operand_entry *output_operand) {
    return_val rv;
    char *parsed_str;
    char *end_ptr;
    long val;

    if (!operand_str || !output_operand)
        return ERR_NULL_PTR;

    /* Initialize output operand */
    output_operand->addressing_mode = 0;
    output_operand->value = 0;
    output_operand->label_str[0] = '\0';

    /* Find operand addressing mode */

    /* Check if Immediate addressing mode */
    if (operand_str[0] == IMMEDIATE_PREFIX_CHAR) {
        /* Copy operand string without prefix into parsed_str */
        parsed_str = malloc_helper(strlen(operand_str) - 1 + 1); /* Additional character for '\0' and one less for IMMEDIATE_PREFIX_CHAR */
        strcpy(parsed_str, operand_str + 1);

        /* If no characters appear after immediate start */
        if (strlen(parsed_str) == 0) {
            free(parsed_str);
            return ERR_IMMEDIATE_EMPTY;
        }

        val = strtol(parsed_str, &end_ptr, 10);

        /* Check if entire string after IMMEDIATE_PREFIX_CHAR was parsed */
        if (*end_ptr != '\0') {
            free(parsed_str);
            return ERR_IMMEDIATE_UNKNOWN;
        }

        /* Check if immediate value range is valid */
        if (val < MIN_IMMEDIATE_VAL || val > MAX_IMMEDIATE_VAL) {
            free(parsed_str);
            return ERR_IMMEDIATE_OUT_OF_RANGE;
        }

        free(parsed_str);

        /* Otherwise, immediate is valid and operand entry is returned */

        output_operand->addressing_mode = IMMEDIATE_MODE;
        output_operand->value = (short) val;
        return OK;
    }

    /* Check relative addressing mode */
    else if (operand_str[0] == RELATIVE_PREFIX_CHAR) {
        /* Copy operand string without prefix into parsed_str */
        parsed_str = malloc_helper(strlen(operand_str) - 1 + 1); /* Additional character for '\0' and one less for RELATIVE_PREFIX_CHAR */
        strcpy(parsed_str, operand_str + 1);

        /* If no characters appear after relative start */
        if (strlen(parsed_str) == 0) {
            free(parsed_str);
            return ERR_RELATIVE_EMPTY;
        }

        rv = is_valid_symbol_name(parsed_str);
        if (rv != OK) {
            free(parsed_str);
            return rv;
        }

        /* Otherwise, symbol name is valid */
        output_operand->addressing_mode = RELATIVE_MODE;
        strcpy(output_operand->label_str, parsed_str);
        free(parsed_str);

        return OK;
    }

    /* Check register addressing mode */
    else if (is_register_keyword(operand_str)) {
        /* Register keyword is of the form r[0-7] */
        output_operand->addressing_mode = REGISTER_MODE;
        output_operand->value = (short) (operand_str[1] - '0');
        return OK;
    }

    /* Otherwise, addressing mode could only be direct mode */
    else {
        /* Copy operand string into parsed_str */
        parsed_str = malloc_helper(strlen(operand_str) + 1); /* Additional character for '\0' */
        strcpy(parsed_str, operand_str);

        if (strlen(parsed_str) == 0) {
            free(parsed_str);
            return ERR_EMPTY_OPERAND;
        }


        rv = is_valid_symbol_name(parsed_str);
        if (rv != OK) {
            free(parsed_str);
            return rv;
        }
        /* Otherwise, symbol name is valid */

        output_operand->addressing_mode = DIRECT_MODE;
        strcpy(output_operand->label_str, parsed_str);
        free(parsed_str);

        return OK;
    }
}


return_val is_valid_instruction_line(const char *line, int label_flag, const instruction_entry **output_instruction,
                                     operand_entry *output_operand_src, operand_entry *output_operand_dst) {
    return_val rv;
    int i;
    const char *offset_line; /* line string after offset */
    char *label_string;

    const instruction_entry *temp_inst_entry;
    operand_entry *curr_operand_entry;
    unsigned int operand_count;

    char *token;
    const char *delims = " ,\t\n";  /* Delimiters for strtok */
    char *line_duplicate;   /* Line duplicate to perform strtok string manipulation */

    if (!line || !output_instruction || !output_operand_src || !output_operand_dst)
        return ERR_NULL_PTR;

    /* Initialize output instruction entry */
    *output_instruction = NULL;

    /* If line contains a label definition, change line offset to skip it */
    if (label_flag == 1) {
        /* Get the full label string */
        rv = get_nth_token(line, 1, &label_string);
        if (rv != OK)
            return rv;
        /* If label isn't set */
        if (!label_string)
            return ERR_NULL_PTR;
        /* Point offset line to the character after the end of the label string */
        offset_line = (const char *) (strstr(line, label_string) + strlen(label_string));
        free(label_string);
    }
    else
        offset_line = line;

    rv = get_nth_token(offset_line, 1, &token);
    if (rv != OK)
        return rv;
        /* If the first token doesn't exist */
    else if (!token)
        return OK;

    /* If the first/second token is not a valid instruction keyword */
    if (!is_instruction_keyword(token)) {
        free(token);
        return OK;
    }

    /* Otherwise, it is. Retrieve the corresponding entry (shouldn't be freed) */
    temp_inst_entry = get_instruction(token);

    free(token);

    /* From now on, if the line isn't a valid instruction line, it'll be considered an error */

    /* Count the number of operands in the line and check if line format is valid */
    rv = count_parameters(offset_line, &operand_count);
    if (rv != OK)
        return rv;

    /* Comapre actual operand count and the instruction's operand count */
    if (temp_inst_entry->operands_count > operand_count)
        return ERR_MISSING_OPERANDS;
    else if (temp_inst_entry->operands_count < operand_count)
        return ERR_TOO_MANY_OPERANDS;

    /* Otherwise, operand count is correct */

    /* Validate operands and compare their addressing mode to the instruction's details */
    /* Filter white characters and commas to get the actual operand strings */

    line_duplicate = malloc_helper(strlen(offset_line) + 1);
    strcpy(line_duplicate, offset_line);

    strtok(line_duplicate, delims); /* The instruction name - ignored */
    for (i = 1; i <= temp_inst_entry->operands_count; i++) {
        token = strtok(NULL, delims); /* The i-th operand. It exists because operand count is correct */

        /* Note that instruction that have only 1 operand, only use the destination operand */

        if (i == 1 && temp_inst_entry->operands_count == 2)
            curr_operand_entry = output_operand_src;
        else
            curr_operand_entry = output_operand_dst;

        rv = get_operand(token, curr_operand_entry);

        /* If for some reason operand analysis failed (invalid operand or external error) */
        if (rv != OK) {
            free(line_duplicate);
            return rv;
        }

        /* Otherwise, the first or second output operand entry contains the current operand info */

        /* Check if operand addressing mode is supported by the instruction */
        if (i == 1 && temp_inst_entry->operands_count == 2) {
            if ((temp_inst_entry->src_addressing & curr_operand_entry->addressing_mode) == 0) {
                free(line_duplicate);
                return ERR_INVALID_SRC_ADDR_MODE;
            }
        }
        else {
            if ((temp_inst_entry->dest_addressing & curr_operand_entry->addressing_mode) == 0) {
                free(line_duplicate);
                return ERR_INVALID_DEST_ADDR_MODE;
            }
        }

        /* Otherwise, operand is of correct addressing */
    }

    free(line_duplicate);

    /* Line is a valid instruction line and outputs are set */
    *output_instruction = temp_inst_entry;

    return OK;
}
