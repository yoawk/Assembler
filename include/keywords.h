/**
 * keywords.h
 * Header file of keywords.c
 */

#ifndef KEYWORDS_H
#define KEYWORDS_H

/* TODO: implement instruction entry */

/**
 * Checks if a given string is an instruction keyword.
 * @param input_string Given string
 * @return 1 if true, 0 if false.
 */
int is_instruction_keyword(const char *input_string);

/**
 * Checks if a given string is a directive keyword.
 * @param input_string Given string
 * @return 1 if true, 0 if false.
 */
int is_directive_keyword(const char *input_string);

/**
 * Checks if a given string is a register name/keyword.
 * @param input_string Given string
 * @return 1 if true, 0 if false.
 */
int is_register_keyword(const char *input_string);

/**
 * Checks if a given string is a reserved keyword (instruction keyword, directive keyword or register name)
 * @param input_string Given string
 * @return 1 if true, 0 if false.
 */
int is_reserved_keyword(const char *input_string);

#endif