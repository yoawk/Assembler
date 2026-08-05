/**
 * directives.h
 * Header file of directives.c
 */

#ifndef DIRECTIVES_H
#define DIRECTIVES_H

#include "errors.h"
#include "symbols.h"

/* Maximum size of a directive (.data, .string, ...) name size (including '.' and excluding '\0')*/
#define MAX_DIRECTIVE_NAME_SIZE 8

/* Minimum and maximum string length (excluding quotation marks and '\0') of a .string content */
#define MIN_DIRECTIVE_STRING_SIZE 1
#define MAX_DIRECTIVE_STRING_SIZE 63

/* Minimum and maximum length of a .data value list */
#define MIN_DIRECTIVE_DATA_LIST_LENGTH 1
#define MAX_DIRECTIVE_DATA_LIST_LENGTH 32

/* Data list integer values are stored as two's complement in a word,
 * so minimum value is -2^(word_size - 1) and maximum is 2^(word_size -1) - 1 */
#define MIN_DIRECTIVE_DATA_LIST_VALUE ( -((1 << (WORD_SIZE - 1)) ))
#define MAX_DIRECTIVE_DATA_LIST_VALUE ( ( 1 << (WORD_SIZE - 1) ) - 1 )

typedef struct {
    /* The directive's name (.data, .string, ...) */
    char directive_name[MAX_DIRECTIVE_NAME_SIZE + 1];

    /* The relevant data corresponding to the directive */
    union {
        char symbol_name[MAX_SYMBOL_NAME_SIZE + 1];         /* For .extern and .entry */
        char content_string[MAX_DIRECTIVE_STRING_SIZE + 1]; /* For .string */

        /* For .data */
        struct {
            unsigned int integer_count;
            short integer_list[MAX_DIRECTIVE_DATA_LIST_LENGTH];
        } data_integers;
    } directive_data;
} directive_entry;

/**
 * Checks if a given string is a directive keyword.
 * @param input_string Given string
 * @return 1 if true, 0 if false and 0 if input string is NULL.
 */
int is_directive_keyword(const char *input_string);

/**
 * Checks if a given line is a valid directive (.data, .string, .extern, or .entry ) line.
 * In case the line is valid, output directive entry's data and name will be set.
 * If no errors occur, and it's not a directive line, output directive values are undefined.
 * If an error occurs, result and output directive entry are undefined.
 *
 * The valid formats of the different directive lines:
 *
 * The valid format of a .data directive line is:
 *     (optional label)_.data_num1_,_num2_,_ ... _numN_(optional '\n')
 * Where _ indicate optional white characters (' ', '\t'),
 * num1...numN are integers (with optional +/- prefix) of minimum size MIN_DIRECTIVE_DATA_LIST_VALUE
 * and maximum size MAX_DIRECTIVE_DATA_LIST_VALUE.
 * The minimum and maximum integer list length are MIN_DIRECTIVE_DATA_LIST_LENGTH and MAX_DIRECTIVE_DATA_LIST_LENGTH.
 *
 * The valid format of a .string directive is:
 *     (optional label)_.string_"(printable ASCII characters except '"')"_
 * Where _ indicate optional white characters (' ', '\t'),
 * and the content of .string is of minimum size MIN_DIRECTIVE_STRING_SIZE
 * and maximum size MAX_DIRECTIVE_STRING_SIZE.
 *
 * The valid format of a .extern directive is:
 *     (optional label - ignored)_.extern_(symbol)_
 * Where the symbol isn't defined anywhere else (except as .extern, in which case multiple definitions don't matter)
 *
 * The valid format of a .entry directive is:
 *     (optional label - ignored)_.entry_(symbol)_
 * Where the symbol is defined as a label in the assembly code (and necessarily not as .extern).
 *
 * NOTE: Regarding symbols, the function only checks if the symbol name is valid -
 * it doesn't check if the symbol are defined elsewhere and how.
 * Also, the function assumes that the label in line (if exists) is valid.
 *
 * @param line Input line
 * @param label_flag 1 if a valid label is present in line, 0 if there is no label
 * @param output_directive Output directive entry
 * @param result If function finished with no errors, result value will signal if line is a directive line.
 *                  1 if it is, 0 otherwise.
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          .data value list possible errors:
 *              ERR_DIRECTIVE_DATA_LIST_TOO_SMALL,
 *              ERR_DIRECTIVE_DATA_LIST_TOO_BIG,
 *              ERR_DATA_LIST_VALUE_UNKNOWN,
 *              ERR_DATA_LIST_VALUE_TOO_SMALL,
 *              ERR_DATA_LIST_VALUE_TOO_BIG.
 *          .string content errors:
 *              ERR_DIRECTIVE_STRING_NO_START_QUOTE,
 *              ERR_DIRECTIVE_STRING_NO_END_QUOTE,
 *              ERR_DIRECTIVE_STRING_CONTENT_TOO_SMALL,
 *              ERR_DIRECTIVE_STRING_CONTENT_TOO_BIG,
 *              ERR_DIRECTIVE_STRING_INVALID_CHAR,
 *              ERR_DIRECTIVE_STRING_CHARS_AFTER_CONTENT.
 *          .extern content errors:
 *              ERR_DIRECTIVE_EXTERN_MISSING_SYMBOL,
 *              ERR_DIRECTIVE_EXTERN_CHARS_AFTER_SYMBOL.
 *          .entry content errors:
 *              ERR_DIRECTIVE_ENTRY_MISSING_SYMBOL,
 *              ERR_DIRECTIVE_ENTRY_CHARS_AFTER_SYMBOL.
 *          Additionally, on count_parameters, get_nth_token, or is_valid_symbol_name fail, their errors.
 */
return_val is_valid_directive_line(const char *line, int label_flag, directive_entry *output_directive, int *result);

#endif
