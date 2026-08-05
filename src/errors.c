/**
 * errors.c
 * Handles the report of return codes (program errors, input assembly errors,
 * control flow codes and stage info messages).
 */

#include "errors.h"

#include "globals.h"
#include "directives.h"
#include "instructions.h"

#include <stdio.h>
#include <stdlib.h>

/* The different error tags */
#define FATAL_TAG "[ FATAL ERROR ] "
#define ASSEMBLY_TAG "[ ASSEMBLY ERROR ] "
#define INFO_TAG "[ INFO ] "


/**
 * Checks if return code is a fatal (program) error
 * @param code Given return code
 * @return 1 if fatal, 0 otherwise
 */
int is_fatal_error(return_val code);

/**
 * Checks if a return code is a stage code.
 * @param code Given return code
 * @return 1 if stage code, 0 otherwise.
 */
int is_stage_code(return_val code);

/**
 * Prints additional location (file name or file name with line number)
 * @param file_name File name string
 * @param line_number Line number (counted from 1)
 * @return void
 */
void print_location(const char *file_name, unsigned int line_number);


void handle_error(return_val error, const char *file_name, unsigned int line_number) {
    int is_fatal;

    /* Control flow codes don't need error printing */
    if (error == OK || error == REACHED_EOF)
        return;

    /* Stage codes should use the info_message function */
    if (is_stage_code(error))
        return;

    is_fatal = is_fatal_error(error);

    /* Print the correct tag */
    if (is_fatal)
        printf("%s", FATAL_TAG);
    else
        printf("%s", ASSEMBLY_TAG);

    /* Print source location */
    if (!is_fatal)
        print_location(file_name, line_number);

    switch (error) {
        /* ---- Program Errors ---- */

        case ERR_NULL_PTR:
            printf("Unexpected NULL pointer.\n");
            break;
        case ERR_ALLOC_FAILED:
            printf("Memory allocation failed.\n");
            break;
        case ERR_REALLOC_FAILED:
            printf("Memory reallocation failed.\n");
            break;
        case ERR_UNEXPECTED_VALUE:
            printf("Encountered an unexpected internal value.\n");
            break;

        /* File operation errors */

        /* General file */
        case ERR_FAILED_TO_OPEN_FILE:
            printf("Could not open file.\n");
            break;
        case ERR_FAILED_TO_CLOSE_FILE:
            printf("Could not close file.\n");
            break;
        case ERR_FAILED_TO_WRITE_FILE:
            printf("Could not write to file.\n");
            break;
        case ERR_FAILED_TO_REMOVE_FILE:
            printf("Could not remove file.\n");
            break;

        /* .as file */
        case ERR_FAILED_TO_OPEN_AS_FOR_READ:
            printf("Could not open the .as source file for reading.\n");
            break;
        case ERR_FAILED_TO_CLOSE_AS:
            printf("Could not close the .as source file.\n");
            break;

        /* .am file */
        case ERR_FAILED_TO_OPEN_AM_FOR_READ:
            printf("Could not open the .am file for reading.\n");
            break;
        case ERR_FAILED_TO_OPEN_AM_FOR_WRITE:
            printf("Could not open the .am file for writing.\n");
            break;
        case ERR_FAILED_TO_WRITE_AM:
            printf("Write operation to the .am file failed.\n");
            break;
        case ERR_FAILED_TO_CLOSE_AM:
            printf("Could not close the .am file.\n");
            break;
        case ERR_FAILED_TO_REMOVE_AM:
            printf("Could not delete the .am file.\n");
            break;

        /* .ob file */
        case ERR_FAILED_TO_OPEN_OB_FOR_WRITE:
            printf("Could not open the .ob object file for writing.\n");
            break;
        case ERR_FAILED_TO_WRITE_OB:
            printf("Write operation to the .ob object file failed.\n");
            break;
        case ERR_FAILED_TO_CLOSE_OB:
            printf("Could not close the .ob object file.\n");
            break;
        case ERR_FAILED_TO_REMOVE_OB:
            printf("Could not delete the .ob object file.\n");
            break;

        /* .ent file */
        case ERR_FAILED_TO_OPEN_ENT_FOR_WRITE:
            printf("Could not open the .ent entries file for writing.\n");
            break;
        case ERR_FAILED_TO_WRITE_ENT:
            printf("Write operation to the .ent entries file failed.\n");
            break;
        case ERR_FAILED_TO_CLOSE_ENT:
            printf("Could not close the .ent entries file.\n");
            break;
        case ERR_FAILED_TO_REMOVE_ENT:
            printf("Could not delete the .ent entries file.\n");
            break;

        /* .ext file */
        case ERR_FAILED_TO_OPEN_EXT_FOR_WRITE:
            printf("Could not open the .ext externals file for writing.\n");
            break;
        case ERR_FAILED_TO_WRITE_EXT:
            printf("Write operation to the .ext externals file failed.\n");
            break;
        case ERR_FAILED_TO_CLOSE_EXT:
            printf("Could not close the .ext externals file.\n");
            break;
        case ERR_FAILED_TO_REMOVE_EXT:
            printf("Could not delete the .ext externals file.\n");
            break;


        /* ---- Assembly Errors ---- */

        /* Line format */
        case ERR_LINE_TOO_BIG:
            printf("Line exceeds the maximum allowed length of %d characters.\n", MAX_LINE_SIZE);
            break;
        case ERR_UNKNOWN_LINE_TYPE:
            printf("Unrecognized line type. Line isn't a valid instruction, directive, comment, or blank line.\n");
            break;

        /* Macro errors */
        case ERR_MACRO_NAME_DUPLICATE:
            printf("Macro name is already defined.\n");
            break;
        case ERR_MACRO_NAME_RESERVED:
            printf("Macro name is a reserved keyword (instruction name, directive name, or register name).\n");
            break;
        case ERR_MACRO_NAME_EMPTY:
            printf("Macro definition is missing a name after '%s'.\n", MACRO_START);
            break;
        case ERR_CHARS_AFTER_MACRO_START:
            printf("Non-white characters appear after macro definition start.\n");
            break;
        case ERR_CHARS_AFTER_MACRO_END:
            printf("Non-white characters appear after '%s'.\n", MACRO_END);
            break;
        case ERR_CHARS_AFTER_MACRO_REF:
            printf("Non-white characters appear after macro reference (a macro reference must appear alone on its line).\n");
            break;

        /* Symbol/label errors */
        case ERR_SYMBOL_NAME_TOO_BIG:
            printf("Symbol name exceeds the maximum allowed length of %d characters.\n", MAX_SYMBOL_NAME_SIZE);
            break;
        case ERR_SYMBOL_NAME_EMPTY:
            printf("Symbol name is empty.\n");
            break;
        case ERR_SYMBOL_NAME_RESERVED:
            printf("Symbol name is a reserved keyword (instruction name, directive name, or register name).\n");
            break;
        case ERR_SYMBOL_FIRST_CHAR:
            printf("Symbol name must begin with an alphabetic character (a-z or A-Z).\n");
            break;
        case ERR_SYMBOL_NON_ALPHANUMERIC:
            printf("Symbol name contains a character that is not alphanumeric (a-z, A-Z, 0-9).\n");
            break;
        case ERR_SYMBOL_IS_MACRO_NAME:
            printf("Symbol name conflicts with an already-defined macro name.\n");
            break;
        case ERR_SYMBOL_LABEL_ALREADY_EXISTS:
            printf("Label symbol name is already defined elsewhere in this file.\n");
            break;
        case ERR_SYMBOL_EXTERN_ALREADY_DEFINED:
            printf("Symbol declared with .extern is already defined an non-external.\n");
            break;
        case ERR_SYMBOL_ENTRY_IS_EXTERNAL:
            printf("Symbol declared with .entry is already declared as .extern.\n");
            break;
        case ERR_SYMBOL_DOES_NOT_EXIST:
            printf("Symbol not found.\n");
            break;

        /* Instruction errors */
        case ERR_MISSING_OPERANDS:
            printf("Too few operands for this instruction.\n");
            break;
        case ERR_TOO_MANY_OPERANDS:
            printf("Too many operands for this instruction.\n");
            break;
        case ERR_INVALID_ADDR_MODE:
            printf("Invalid addressing mode for operand.\n");
            break;
        case ERR_INVALID_SRC_ADDR_MODE:
            printf("Invalid addressing mode for source operand of this instruction.\n");
            break;
        case ERR_INVALID_DEST_ADDR_MODE:
            printf("Invalid addressing mode for destination operand of this instruction.\n");
            break;

        /* Operand errors */
        case ERR_IMMEDIATE_OUT_OF_RANGE:
            printf("Immediate value is out of range. Valid range is [%d, %d].\n", MIN_IMMEDIATE_VAL,
                   MAX_IMMEDIATE_VAL);
            break;
        case ERR_IMMEDIATE_EMPTY:
            printf("No value after immediate prefix '%c'.\n", IMMEDIATE_PREFIX_CHAR);
            break;
        case ERR_IMMEDIATE_UNKNOWN:
            printf("Invalid immediate value. Must be a decimal integer with optional leading '+' or '-'.\n");
            break;
        case ERR_RELATIVE_EMPTY:
            printf("No symbol name provided after the relative-mode prefix '%c'.\n", RELATIVE_PREFIX_CHAR);
            break;
        case ERR_RELATIVE_IS_EXTERNAL:
            printf("Relative addressing mode cannot be used with an external symbol.\n");
            break;
        case ERR_EMPTY_OPERAND:
            printf("Operand is empty.\n");
            break;

        /* .data directive errors */
        case ERR_DIRECTIVE_DATA_LIST_TOO_SMALL:
            printf(".data directive requires at least one integer value.\n");
            break;
        case ERR_DIRECTIVE_DATA_LIST_TOO_BIG:
            printf(".data directive integer list exceeds the maximum allowed length of %d values.\n", MAX_DIRECTIVE_DATA_LIST_LENGTH);
            break;
        case ERR_DATA_LIST_VALUE_UNKNOWN:
            printf(".data directive contains a value that is not a valid integer.\n");
            break;
        case ERR_DATA_LIST_VALUE_TOO_SMALL:
            printf(".data value is below the minimum allowed value of %d.\n", MIN_DIRECTIVE_DATA_LIST_VALUE);
            break;
        case ERR_DATA_LIST_VALUE_TOO_BIG:
            printf(".data value exceeds the maximum allowed value of %d.\n", MAX_DIRECTIVE_DATA_LIST_VALUE);
            break;

        /* .string directive errors */
        case ERR_DIRECTIVE_STRING_NO_START_QUOTE:
            printf(".string content must be enclosed in double quotes. opening '\"' not found.\n");
            break;
        case ERR_DIRECTIVE_STRING_NO_END_QUOTE:
            printf(".string content is missing its closing double quote '\"'.\n");
            break;
        case ERR_DIRECTIVE_STRING_INVALID_CHAR:
            printf(".string content contains a character that is not a printable ASCII character.\n");
            break;
        case ERR_DIRECTIVE_STRING_CONTENT_TOO_SMALL:
            printf(".string content must contain at least %d character(s).\n", MIN_DIRECTIVE_STRING_SIZE);
            break;
        case ERR_DIRECTIVE_STRING_CONTENT_TOO_BIG:
            printf(".string content exceeds the maximum allowed length of %d characters.\n", MAX_DIRECTIVE_STRING_SIZE);
            break;
        case ERR_DIRECTIVE_STRING_CHARS_AFTER_CONTENT:
            printf("Extra non-whitespace characters appear after the .string closing quote.\n");
            break;

        /* .extern / .entry directive errors */
        case ERR_DIRECTIVE_EXTERN_MISSING_SYMBOL:
            printf(".extern directive requires exactly one symbol name argument.\n");
            break;
        case ERR_DIRECTIVE_EXTERN_CHARS_AFTER_SYMBOL:
            printf("Non-white characters appear after the .extern symbol name.\n");
            break;
        case ERR_DIRECTIVE_ENTRY_MISSING_SYMBOL:
            printf(".entry directive requires exactly one symbol name argument.\n");
            break;
        case ERR_DIRECTIVE_ENTRY_CHARS_AFTER_SYMBOL:
            printf("Non-white characters appear after the .entry symbol name.\n");
            break;

        /* Parameter lists errors (for both operands and directives) */
        case ERR_NO_NAME:
            printf("Expected an instruction name or directive keyword at the start of line.\n");
            break;
        case ERR_EARLY_COMMA:
            printf("Comma appears before the first parameter.\n");
            break;
        case ERR_MULTIPLE_COMMAS:
            printf("Multiple consecutive commas between parameters.\n");
            break;
        case ERR_EXTRA_COMMA:
            printf("Trailing comma after the last parameter.\n");
            break;
        case ERR_MISSING_COMMA:
            printf("Missing comma separator between two parameters.\n");
            break;

        default:
            printf("Unknown return code: %d.\n", error);
            break;
    }

    /* Fatal errors terminate the process immediately */
    if (is_fatal) {
        printf("Fatal error occurred, exiting.\n");
        exit(EXIT_FAILURE);
    }
}


int is_fatal_error(return_val code) {
    switch (code) {
        case ERR_NULL_PTR:
        case ERR_ALLOC_FAILED:
        case ERR_REALLOC_FAILED:
        case ERR_UNEXPECTED_VALUE:
        case ERR_FAILED_TO_OPEN_FILE:
        case ERR_FAILED_TO_CLOSE_FILE:
        case ERR_FAILED_TO_WRITE_FILE:
        case ERR_FAILED_TO_REMOVE_FILE:
        case ERR_FAILED_TO_OPEN_AS_FOR_READ:
        case ERR_FAILED_TO_CLOSE_AS:
        case ERR_FAILED_TO_OPEN_AM_FOR_READ:
        case ERR_FAILED_TO_OPEN_AM_FOR_WRITE:
        case ERR_FAILED_TO_WRITE_AM:
        case ERR_FAILED_TO_CLOSE_AM:
        case ERR_FAILED_TO_REMOVE_AM:
        case ERR_FAILED_TO_OPEN_OB_FOR_WRITE:
        case ERR_FAILED_TO_WRITE_OB:
        case ERR_FAILED_TO_CLOSE_OB:
        case ERR_FAILED_TO_REMOVE_OB:
        case ERR_FAILED_TO_OPEN_ENT_FOR_WRITE:
        case ERR_FAILED_TO_WRITE_ENT:
        case ERR_FAILED_TO_CLOSE_ENT:
        case ERR_FAILED_TO_REMOVE_ENT:
        case ERR_FAILED_TO_OPEN_EXT_FOR_WRITE:
        case ERR_FAILED_TO_WRITE_EXT:
        case ERR_FAILED_TO_CLOSE_EXT:
        case ERR_FAILED_TO_REMOVE_EXT:
            return 1;
        default:
            return 0;
    }
}

int is_stage_code(return_val code) {
    switch (code) {
        case PRE_ASSEMBLER_START:
        case FIRST_PASS_START:
        case SECOND_PASS_START:
        case PRE_ASSEMBLER_FAIL:
        case FIRST_PASS_FAIL:
        case SECOND_PASS_FAIL:
        case PRE_ASSEMBLER_SUCCESS:
        case FIRST_PASS_SUCCESS:
        case SECOND_PASS_SUCCESS:
            return 1;
        default:
            return 0;
    }
}

void print_location(const char *file_name, unsigned int line_number) {
    if (file_name && line_number > 0)
        printf("In %s, line %u: ", file_name, line_number);
    else if (file_name)
        printf("In %s: ", file_name);

    return;
}
