/**
 * errors.h
 * Header file of errors.c
 * Includes definition of all return codes.
 */

/**
 * Error codes are divided into four groups:
 *
 *  Control Flow Codes (non-error):
 *      Non-error codes that are used for control flow management between functions.
 *
 *  Program Errors (fatal):
 *      Indicate an internal error or input/output fails.
 *      Will result in program termination.
 *
 *  Stage Codes (info messages):
 *      Used for assembler stage info messages (stage failed/succeeded, etc.)
 *
 *  Assembly Errors (non-fatal):
 *     Indicate a mistake (for example syntax error) in the assembly source file.
 *     Will only produce a diagnostic error message with error info.
 *
 */

#ifndef ERRORS_H
#define ERRORS_H


/* The project's return codes type */
typedef enum {
    /* ---- Control Flow Codes ---- */

    OK = 0,      /* Success code */
    REACHED_EOF, /* Reached End Of File */

    /* ---- Program Errors ---- */

    /* General program errors */
    ERR_NULL_PTR,         /* Unexpected NULL pointer */
    ERR_ALLOC_FAILED,     /* Memory allocation failed - error is severe and execution should be terminated */
    ERR_REALLOC_FAILED,   /* Memory reallocation failed */
    ERR_UNEXPECTED_VALUE, /* Found an unexpected value */

    /* File/line handling errors */
    ERR_FAILED_TO_OPEN_FILE,   /* File opening failed */
    ERR_FAILED_TO_CLOSE_FILE,  /* File closing failed */
    ERR_FAILED_TO_WRITE_FILE,  /* File writing failed */
    ERR_FAILED_TO_REMOVE_FILE, /* File removal failed */

    /* File-specific errors */

    /* .as file*/
    ERR_FAILED_TO_OPEN_AS_FOR_READ, /* Failed to open .as file for reading */
    ERR_FAILED_TO_CLOSE_AS,         /* Failed to close .as file */

    /* .am file */
    ERR_FAILED_TO_OPEN_AM_FOR_READ,  /* Failed to open .am file for reading */
    ERR_FAILED_TO_OPEN_AM_FOR_WRITE, /* Failed to open .am file for writing */
    ERR_FAILED_TO_WRITE_AM,          /* Failed to write to .am file */
    ERR_FAILED_TO_CLOSE_AM,          /* Failed to close .am file */
    ERR_FAILED_TO_REMOVE_AM,         /* Failed to remove .am file */

    /* .ob file */
    ERR_FAILED_TO_OPEN_OB_FOR_WRITE, /* Failed to open .ob file for writing */
    ERR_FAILED_TO_WRITE_OB,          /* Failed to write to .ob file */
    ERR_FAILED_TO_CLOSE_OB,          /* Failed to close .ob file */
    ERR_FAILED_TO_REMOVE_OB,         /* Failed to remove .ob file */

    /* .ent file */
    ERR_FAILED_TO_OPEN_ENT_FOR_WRITE, /* Failed to open .ent file for writing */
    ERR_FAILED_TO_WRITE_ENT,          /* Failed to write to .ent file */
    ERR_FAILED_TO_CLOSE_ENT,          /* Failed to close .ent file */
    ERR_FAILED_TO_REMOVE_ENT,         /* Failed to remove .ent file */

    /* .ext file */
    ERR_FAILED_TO_OPEN_EXT_FOR_WRITE, /* Failed to open .ext file for writing */
    ERR_FAILED_TO_WRITE_EXT,          /* Failed to write to .ext file */
    ERR_FAILED_TO_CLOSE_EXT,          /* Failed to close .ext file */
    ERR_FAILED_TO_REMOVE_EXT,         /* Failed to remove .ext file */


    /* ---- Stage Codes ---- */

    /* Stage start */
    PRE_ASSEMBLER_START, /* Pre-assembler stage started */
    FIRST_PASS_START,    /* First pass stage started */
    SECOND_PASS_START,   /* Second pass stage started */

    /* Stage failure */
    PRE_ASSEMBLER_FAIL, /* Pre-assembler stage failed */
    FIRST_PASS_FAIL,    /* First pass stage failed */
    SECOND_PASS_FAIL,   /* Second pass stage failed */

    /* Stage completion */
    PRE_ASSEMBLER_SUCCESS, /* Pre-assembler stage finished successfully */
    FIRST_PASS_SUCCESS,    /* First pass stage finished successfully */
    SECOND_PASS_SUCCESS,   /* Second pass stage finished successfully */



    /* ---- Assembly Errors ---- */

    /* Line errors */
    ERR_LINE_TOO_BIG,      /* Line in input file exceeds MAX_LINE_SIZE */
    ERR_UNKNOWN_LINE_TYPE, /* Line wasn't identified as an empty, comment, instruction or directive line */

    /* Macro errors */
    ERR_MACRO_NAME_DUPLICATE,    /* Macro name in a macro definition start is an existing macro name */
    ERR_MACRO_NAME_RESERVED,     /* Macro name in macro definition start is a reserved keyword (instruction, directive, register) */
    ERR_MACRO_NAME_EMPTY,        /* Macro name in macro definition start is empty*/
    ERR_CHARS_AFTER_MACRO_START, /* Non-white characters appear after macro definition start */
    ERR_CHARS_AFTER_MACRO_END,   /* Non-white characters appear after macro definition end */
    ERR_CHARS_AFTER_MACRO_REF,   /* Non-white characters appear after macro reference */

    /* Symbol/Label errors */
    ERR_SYMBOL_NAME_TOO_BIG,           /* Symbol name exceeds MAX_SYMBOL_NAME_SIZE */
    ERR_SYMBOL_NAME_EMPTY,             /* Symbol name is empty */
    ERR_SYMBOL_NAME_RESERVED,          /* Symbol name collides with a reserved keyword */
    ERR_SYMBOL_FIRST_CHAR,             /* First character of symbol name is not a letter (upper or lower) */
    ERR_SYMBOL_NON_ALPHANUMERIC,       /* Symbol name contains non-alphanumeric characters */
    ERR_SYMBOL_IS_MACRO_NAME,          /* Symbol name is a defined macro's name */
    ERR_SYMBOL_LABEL_ALREADY_EXISTS,   /* A label's symbol name is already defined elsewhere */
    ERR_SYMBOL_EXTERN_ALREADY_DEFINED, /* A .extern's symbol name is already defined as a non-extern symbol */
    ERR_SYMBOL_ENTRY_IS_EXTERNAL,      /* A .entry symbol name is already defined as an external symbol */
    ERR_SYMBOL_DOES_NOT_EXIST,         /* A referenced symbol does not exist */

    /* Instruction errors */
    ERR_MISSING_OPERANDS,       /* Instruction expected more operands*/
    ERR_TOO_MANY_OPERANDS,      /* Instruction expected fewer operands */
    ERR_INVALID_ADDR_MODE,      /* Encountered some operand of an invalid addressing mode */
    ERR_INVALID_SRC_ADDR_MODE,  /* The source operand is of an invalid addressing mode */
    ERR_INVALID_DEST_ADDR_MODE, /* The destination operand is of an invalid addressing mod */


    /* Operands errors */
    ERR_IMMEDIATE_OUT_OF_RANGE, /* Immediate value is out of valid range */
    ERR_IMMEDIATE_EMPTY,        /* No characters after the IMMEDIATE_PREFIX_CHAR */
    ERR_IMMEDIATE_UNKNOWN,      /* Couldn't identify immediate value */
    ERR_RELATIVE_EMPTY,         /* Relative symbol is empty */
    ERR_RELATIVE_IS_EXTERNAL,   /* An operand with relative addressing mode is an external symbol */
    ERR_EMPTY_OPERAND,          /* Operand is empty */

    /* Directive .data errors */
    ERR_DIRECTIVE_DATA_LIST_TOO_SMALL, /* Data directive's integers list is too small */
    ERR_DIRECTIVE_DATA_LIST_TOO_BIG,   /* Data directive's integers list exceeds max length */
    ERR_DATA_LIST_VALUE_UNKNOWN,       /* A data directive's integer is of unknown invalid value */
    ERR_DATA_LIST_VALUE_TOO_SMALL,     /* A .data directive's integer value is too small */
    ERR_DATA_LIST_VALUE_TOO_BIG,       /* A .data directive's integer value is too big*/

    /* Directive .string errors */
    ERR_DIRECTIVE_STRING_NO_START_QUOTE,      /* A .string's content doesn't start with '"' */
    ERR_DIRECTIVE_STRING_NO_END_QUOTE,        /* A .string's content doesn't end with '"' */
    ERR_DIRECTIVE_STRING_INVALID_CHAR,        /* A .string's content string contains a character which is not a printable ASCII character */
    ERR_DIRECTIVE_STRING_CONTENT_TOO_SMALL,   /* A .string's content string is too small */
    ERR_DIRECTIVE_STRING_CONTENT_TOO_BIG,     /* A .string's content string is too big */
    ERR_DIRECTIVE_STRING_CHARS_AFTER_CONTENT, /* A non-white character appeared after the .string content string */

    /* Directive .extern errors */
    ERR_DIRECTIVE_EXTERN_MISSING_SYMBOL,     /* A .extern directive line is missing a symbol */
    ERR_DIRECTIVE_EXTERN_CHARS_AFTER_SYMBOL, /* A .extern line has non-white characters after symbol */

    /* directive .entry errors */
    ERR_DIRECTIVE_ENTRY_MISSING_SYMBOL,     /* A .entry directive line is missing a symbol */
    ERR_DIRECTIVE_ENTRY_CHARS_AFTER_SYMBOL, /* A .entry directive line has non-white characters after symbol */

    /* Parameter lists errors (for both operands and directives) */
    ERR_NO_NAME,         /* First string sequence wasn't found */
    ERR_EARLY_COMMA,     /* Comma before any parameters */
    ERR_MULTIPLE_COMMAS, /* Multiple commas instead of one */
    ERR_EXTRA_COMMA,     /* A comma after last parameter */
    ERR_MISSING_COMMA    /* No comma between parameters */
} return_val;


/**
 * Reports errors to stdout and, for fatal program errors, terminates the program.
 * Should be used for program errors and assembly errors only.
 * For input assembly errors, file name and line number can be printed for additional context.
 * @param error The error code to report
 * @param file_name Name of the source file in which the error occurred. Will be ignored if not needed.
 * @param line_number Line number (count from 1) where the error was detected. Will be ignored if not needed.
 */
void handle_error(return_val error, const char *file_name, unsigned int line_number);

/**
 * Prints info messages to stdout regarding assembler stage.
 * @param code Stage code
 * @param file_name File name string
 */
void info_message(return_val code, const char *file_name);

#endif
