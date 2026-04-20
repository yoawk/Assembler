/**
 * keywords.h
 * Header file of keywords.c
 */

#ifndef KEYWORDS_H
#define KEYWORDS_H

/**
 * Checks if a given string is a directive keyword.
 * @param input_string Given string
 * @return 1 if true, 0 if false and 0 if input string is NULL.
 */
int is_directive_keyword(const char *input_string);

/**
 * Checks if a given string is a register name/keyword.
 * @param input_string Given string
 * @return 1 if true, 0 if false and 0 if input string is NULL.
 */
int is_register_keyword(const char *input_string);

/**
 * Checks if a given string is a reserved keyword (instruction keyword, directive keyword or register name)
 * @param input_string Given string
 * @return 1 if true, 0 if false and 0 if input string is NULL.
 */
int is_reserved_keyword(const char *input_string);

#endif
