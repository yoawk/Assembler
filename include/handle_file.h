/**
 * handle_file.h
 * Header file of handle_file.c
 */

#ifndef HANDLE_FILE_H
#define HANDLE_FILE_H

#include "errors.h"

#include <stdio.h>

/**
 * Reads and returns a line from a stream using a file descriptor.
 * Expects stream to be non-empty.
 * Output line might not contain '\n' if EOF has been reached.
 * @param output_line Pointer to output line string.
 *                      Assumes *output_line points to an allocated memory block of
 *                      at least max_size + 2 bytes (for '\n' and '\0' characters)
 * @param max_size Maximum size of a read line (excluding '\n' and '\0')
 * @param fp Given file descriptor
 * @return OK on success, ERR_NULL_PTR on NULL pointer, ERR_EOF if EOF is , ERR_LINE_TOO_BIG
 */
return_val get_line(char *output_line, size_t max_size, FILE *fp);

/**
 *
 * @param file_name
 * @param file_extension
 * @param mode
 * @param fp
 * @return
 */
return_val fopen_helper(const char *file_name, const char *file_extension, const char *mode, FILE **fp);

/**
 * Checks if a given line is empty, meaning it only contains
 * ' ', '\t', '\n' characters.
 * @param line Input line
 * @return 1 if line is empty, 0 otherwise.
 */
int is_empty_line(const char *line);


/**
 * Get the nth non-white character (not ' ', '\t', '\n') sequence (token) of a line.
 * If nth token is found, token parameter will be set to point at the dynamically allocated nth token string.
 * If nth token doesn't exist, token will be set to point at NULL.
 * Output token string will be allocated only if nth token is found with no error.
 * @param line Given line
 * @param n Token count (starting from 1)
 * @param token Pointer to output token string
 * @return OK on success, ERR_NULL_PTR on NULL pointer and ERR_ALLOC_FAILED on allocation fail.
 */
return_val get_nth_token(const char *line, int n, char **token);

#endif