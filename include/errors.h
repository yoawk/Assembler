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
