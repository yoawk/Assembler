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
 * @return OK on success, ERR_NULL_PTR on NULL pointer, REACHED_EOF if no bytes left to read,
 *          ERR_LINE_TOO_BIG if line size is bigger than max_size.
 */
return_val get_line(char *output_line, size_t max_size, FILE *fp);

/**
 * Helper function for the fopen utility.
 * Opens a file appended by an extension with the given file access mode.
 * @param file_name File name
 * @param file_extension File extension (empty for non)
 * @param mode File access mode
 * @param fp Pointer to the result file descriptor
 * @return OK on success. ERR_NULL_PTR, or ERR_FAILED_TO_OPEN_FILE on corresponding errors.
 */
return_val fopen_helper(const char *file_name, const char *file_extension, const char *mode, FILE **fp);

/**
 * Helper function for the remove file function.
 * Removes/Deletes a file appended by an extension.
 * @param file_name File name
 * @param file_extension File extension (empty for non)
 * @return OK on success. ERR_NULL_PTR, or ERR_FAILED_TO_REMOVE_FILE on corresponding errors.
 */
return_val remove_helper(const char *file_name, const char *file_extension);

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
 * @return OK on success, ERR_NULL_PTR on unexpected NULL pointer.
 */
return_val get_nth_token(const char *line, int n, char **token);

/**
 * Validates a line of a string (called name) followed by a sequence
 * of parameters, and counts parameters.
 * Valid format is as follows ("_" means one or more white characters: ' '/'\t'/'\n')
 * name _param1_,_param2_,_param3 ... _,_paramN_
 *
 * @param line Input line
 * @param output_count Pointer to output count of parameters
 * @return OK on success (valid input). ERR_NULL_PTR on NULL pointer.
 *          ERR_EARLY_COMMA on comma before reaching a parameter,
 *          ERR_MULTIPLE_COMMAS for multiple commas together,
 *          ERR_MISSING_COMMA for a missing comma,
 *          ERR_NO_NAME if name string is missing,
 *          ERR_EXTRA_COMMA for a comma after last parameter.
 */
return_val count_parameters(const char *line, unsigned int *output_count);

#endif