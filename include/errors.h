/**
 * errors.h
 * Header file of errors.c
 * Defines return values and error messages
 */

#ifndef ERRORS_H
#define ERRORS_H

/* The project's return value type */
typedef enum {
    OK = 0,
    ERR_NULL_PTR,
    ERR_ALLOC_FAILED,
    ERR_EOF,

    ERR_INVALID_FILE_NAME,
    ERR_FAILED_TO_OPEN_FILE,

    ERR_LINE_TOO_BIG,

    ERR_INVALID_MACRO_NAME,


} return_val;


/**
 * Prints error message of given error to stdout.
 * @param error Given error
 * @param line_number Line number where error occurred (first line number is 1).
 *                      If error is not line related, line_number will be ignored.
 */
void print_error(return_val error, unsigned int line_number);

#endif