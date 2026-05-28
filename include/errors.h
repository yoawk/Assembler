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

    /* Stage errors */
    ERR_PRE_ASSEMBLER_FAIL, /* Pre-assembler stage failed */
    ERR_FIRST_PASS_FAIL,    /* First pass stage failed */
    ERR_SECOND_PASS_FAIL,   /* Second pass stage failed */

    /* General errors */
    ERR_NULL_PTR,     /* Unexpected NULL pointer */
    ERR_ALLOC_FAILED, /* Memory allocation failed */

    /* File/line handling errors */
    ERR_INVALID_FILE_NAME,     /* TODO: Might be unnecessary */
    ERR_FAILED_TO_OPEN_FILE,   /* File opening failed */
    ERR_FAILED_TO_WRITE_FILE,  /* File writing failed */
    ERR_FAILED_TO_REMOVE_FILE, /* File removal failed */
    ERR_LINE_TOO_BIG,          /* Line in input file exceeds MAX_LINE_SIZE */
    ERR_ONLY_EOF,              /* No bytes left to read */

    /* Macro errors */
    ERR_MACRO_NAME_DUPLICATE,    /* Macro name in a macro definition start is an existing macro name */
    ERR_MACRO_NAME_RESERVED,     /* Macro name in macro definition start is a reserved keyword (instruction, directive, register) */
    ERR_MACRO_NAME_EMPTY,        /* Macro name in macro definition start is empty*/
    ERR_CHARS_AFTER_MACRO_START, /* Non-white characters appear after macro definition start */
    ERR_CHARS_AFTER_MACRO_END,   /* Non-white characters appear after macro definition end */
    ERR_CHARS_AFTER_MACRO_REF    /* Non-white characters appear after macro reference */
} return_val;


/**
 * Prints an error message of a given error to stdout.
 * @param error Given error
 * @param context Additional context string. If context is not needed, it'll be ignored.
 * @param file_name Name of the file in which the error occurred. If not needed, it'll be ignored.
 * @param line_number Line number where error occurred (first line number is 1).
 *                      If error is not line related, line_number will be ignored.
 */
void print_error(return_val error, const char *context, const char *file_name, unsigned int line_number);

#endif
