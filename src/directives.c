/**
 * directives.c
 * Handles directive lines validation and parsing.
 */

#include "directives.h"

#include "handle_file.h"
#include "utils.h"
#include "globals.h"

#include <stdlib.h>
#include <string.h>


/* Bounds of ASCII printable characters */
#define ASCII_PRINT_START 32
#define ASCII_PRINT_END 126

/**
 * Checks if a line that begins with a ".data" keyword (and without a label) is a valid .data directive line.
 * Behavior is undefined if input line doesn't contain a ".data" keyword and or contains a label definition.
 * @param input_line Input line
 * @param output_directive Output directive entry
 * @return OK on success.
 *          ERR_NULL_PTR on unexpected NULL pointer.
 *          .data value list possible errors:
 *              ERR_DIRECTIVE_DATA_LIST_TOO_SMALL,
 *              ERR_DIRECTIVE_DATA_LIST_TOO_BIG,
 *              ERR_DATA_LIST_VALUE_UNKNOWN,
 *              ERR_DATA_LIST_VALUE_TOO_SMALL,
 *              ERR_DATA_LIST_VALUE_TOO_BIG.
 *          On count_parameters fail, its errors.
 */
return_val validate_data_directive(const char *input_line, directive_entry *output_directive);

/**
 * Checks if a line that begins with a ".string" keyword (and without a label) is a valid .string directive line.
 * Behavior is undefined if input line doesn't contain a ".string" keyword and or contains a label definition.
 * @param input_line Input line
 * @param output_directive Output directive entry
 * @return OK on success.
 *          ERR_NULL_PTR on unexpected NULL pointer.
 *          .string content errors:
 *              ERR_DIRECTIVE_STRING_NO_START_QUOTE,
 *              ERR_DIRECTIVE_STRING_NO_END_QUOTE,
 *              ERR_DIRECTIVE_STRING_CONTENT_TOO_SMALL,
 *              ERR_DIRECTIVE_STRING_CONTENT_TOO_BIG,
 *              ERR_DIRECTIVE_STRING_INVALID_CHAR,
 *              ERR_DIRECTIVE_STRING_CHARS_AFTER_CONTENT.
 *
 */
return_val validate_string_directive(const char *input_line, directive_entry *output_directive);

/**
 * Checks if a line that begins with a ".extern" keyword (and without a label) is a valid .extern directive line.
 * Behavior is undefined if input line doesn't contain a ".extern" keyword and or contains a label definition.
 * @param input_line Input line
 * @param output_directive Output directive entry
 * @return OK on success.
 *          ERR_NULL_PTR on unexpected NULL pointer.
 *          .extern content errors:
 *              ERR_DIRECTIVE_EXTERN_MISSING_SYMBOL,
 *              ERR_DIRECTIVE_EXTERN_CHARS_AFTER_SYMBOL.
 *          Additionally, on get_nth_token or is_valid_symbol_name fail, their errors.
 */
return_val validate_extern_directive(const char *input_line, directive_entry *output_directive);

/**
 * Checks if a line that begins with a ".entry" keyword (and without a label) is a valid .entry directive line.
 * Behavior is undefined if input line doesn't contain a ".entry" keyword and or contains a label definition.
 * @param input_line Input line
 * @param output_directive Output directive entry
 * @return OK on success.
 *          ERR_NULL_PTR on unexpected NULL pointer.
 *          .entry content errors:
 *              ERR_DIRECTIVE_ENTRY_MISSING_SYMBOL,
 *              ERR_DIRECTIVE_ENTRY_CHARS_AFTER_SYMBOL.
 *          Additionally, on get_nth_token or is_valid_symbol_name fail, their errors.
 */
return_val validate_entry_directive(const char *input_line, directive_entry *output_directive);

int is_directive_keyword(const char *input_string) {
    const char *const directives_table[] = {".data", ".string", ".entry", ".extern"};
    const size_t directives_count = sizeof(directives_table) / sizeof(directives_table[0]);
    size_t i;

    if (!input_string)
        return 0;

    for (i = 0; i < directives_count; i++)
        if (strcmp(directives_table[i], input_string) == 0)
            return 1;

    return 0;
}

return_val is_valid_directive_line(const char *line, int label_flag, directive_entry *output_directive, int *result) {
    return_val rv;
    const char *offset_line;
    char *label_string;
    char *directive_string;

    if (!line || !output_directive || !result)
        return ERR_NULL_PTR;

    /* Initialize output to avoid caller misuse */
    *result = 0;
    (output_directive->directive_name)[0] = '\0';

    /* If line contains a label definition, change offset line to skip it */
    if (label_flag == 1) {
        /* Get the full label string */
        rv = get_nth_token(line, 1, &label_string);
        if (rv != OK)
            return rv;
        /* If label doesn't exist */
        if (!label_string)
            return ERR_NULL_PTR;

        /* Point offset line to the character after the end of the label string */
        offset_line = (const char *) (strstr(line, label_string) + strlen(label_string));
        free(label_string);
    }
    else
        offset_line = line;

    /* Extract the first (or second, if a label is present) token of the line. It should be the directive keyword. */
    rv = get_nth_token(offset_line, 1, &directive_string);
    if (rv != OK) {
        return rv;
    }
    /* If the first token doesn't exist, then line is not a directive line */
    else if (!directive_string) {
        *result = 0;
        return OK;
    }


    /* If directive is the .data */
    if (strcmp(directive_string, ".data") == 0) {
        free(directive_string);
        /* From now on, if the line isn't a valid directive line, it'll be considered an error */

        /* Validate the data directive's value list.
         * Helper function also sets output directive entry's appropriate values */
        rv = validate_data_directive(offset_line, output_directive);
        if (rv != OK)
            return rv;

        /* If validation was successful, then output directive is set and line validation is finished */
        *result = 1;
        return OK;
    }

    /* If directive is .string */
    else if (strcmp(directive_string, ".string") == 0) {
        free(directive_string);
        /* From now on, if the line isn't a valid directive line, it'll be considered an error */

        /* Validate the string's content.
         * Helper function also sets output directive entry's appropriate values (on success) */
        rv = validate_string_directive(offset_line, output_directive);
        if (rv != OK)
            return rv;

        /* If validation was successful, then output directive is set and line validation is finished */
        *result = 1;
        return OK;
    }

    /* If directive is .extern */
    else if (strcmp(directive_string, ".extern") == 0) {
        free(directive_string);
        /* From now on, if the line isn't a valid directive line, it'll be considered an error */

        /* Validate the rest of the line (after .extern keyword) */
        /* Helper function also sets output directive entry's appropriate values (on success) */
        rv = validate_extern_directive(offset_line, output_directive);
        if (rv != OK)
            return rv;

        /* If no error occurred, then line is a valid .extern directive line
         * and output directive entry was set accordingly */
        *result = 1;
        return OK;
    }

    /* If directive is .entry */
    else if (strcmp(directive_string, ".entry") == 0) {
        free(directive_string);
        /* From now on, if the line isn't a valid directive line, it'll be considered an error */

        /* Validate the rest of the line (after .entry keyword) */
        /* Helper function also sets output directive entry's appropriate values (on success) */
        rv = validate_entry_directive(offset_line, output_directive);
        if (rv != OK)
            return rv;

        /* If no error occurred, then line is a valid .entry directive line
         * and output directive entry was set accordingly */
        *result = 1;
        return OK;
    }

    /* Otherwise, first token isn't a directive keyword, and therefore line isn't a directive line */
    else {
        free(directive_string);
        *result = 0;
        return OK;
    }
}

return_val validate_data_directive(const char *input_line, directive_entry *output_directive) {
    return_val rv;
    unsigned int i;
    const char *delims;
    char *line_duplicate;
    char *token, *end_ptr;
    unsigned int integers_count;
    long integer_val;

    if (!input_line || !output_directive)
        return ERR_NULL_PTR;

    /* Set output directive entry's name */
    strcpy(output_directive->directive_name, ".data");

    /* Count the number of integers in the line and check if line format is valid */
    rv = count_parameters(input_line, &integers_count);
    if (rv != OK)
        return rv;

    /* Check integers count validity */
    if (integers_count < MIN_DIRECTIVE_DATA_LIST_LENGTH)
        return ERR_DIRECTIVE_DATA_LIST_TOO_SMALL;
    else if (integers_count > MAX_DIRECTIVE_DATA_LIST_LENGTH)
        return ERR_DIRECTIVE_DATA_LIST_TOO_BIG;


    /* Create a line duplicate for strtok */
    line_duplicate = malloc_helper(strlen(input_line) + 1);
    strcpy(line_duplicate, input_line);

    /* Check if integer list values are valid (note that count_parameters validated the 'list format') */
    delims = " ,\t\n";
    strtok(line_duplicate, delims); /* Skip the .data string */
    for (i = 0; i < integers_count; i++) {
        token = strtok(NULL, delims);

        /* Check if current list element is a valid .data integer value */

        /* Check if it's a valid integer */
        integer_val = strtol(token, &end_ptr, 10);

        /* Check if entire string was converted (meaning it's a valid long value) */
        if (*end_ptr != '\0') {
            free(line_duplicate);
            return ERR_DATA_LIST_VALUE_UNKNOWN;
        }

        /* Check if integer value is in the data list integer value bounds */
        if (integer_val < MIN_DIRECTIVE_DATA_LIST_VALUE) {
            free(line_duplicate);
            return ERR_DATA_LIST_VALUE_TOO_SMALL;
        }
        else if (integer_val > MAX_DIRECTIVE_DATA_LIST_VALUE) {
            free(line_duplicate);
            return ERR_DATA_LIST_VALUE_TOO_BIG;
        }

        /* Current list element is a valid .data integer value.
         * Aad it to list */
        (output_directive->directive_data).data_integers.integer_list[i] = ((short) integer_val & WORD_MASK);
    }

    free(line_duplicate);

    /* Set directive entry's .data integer count */
    (output_directive->directive_data).data_integers.integer_count = integers_count;
    return OK;
}

return_val validate_string_directive(const char *input_line, directive_entry *output_directive) {
    const char *c;
    int i;

    if (!input_line || !output_directive)
        return ERR_NULL_PTR;

    /* Set output directive entry's name */
    strcpy(output_directive->directive_name, ".string");

    /* Point c to the character after the .string keyword */
    c = (const char *) strstr(input_line, ".string");
    c += strlen(".string");

    /* Go over optional white characters before the .string content string */
    while (*c == ' ' || *c == '\t')
        c++;

    /* If encountered a non-white character that is not '"' */
    if (*c != '\"')
        return ERR_DIRECTIVE_STRING_NO_START_QUOTE;

    /* Otherwise, we begin going over the string content */
    c++;

    i = 0;
    /* Go over string content and update the output directive entry content string
     * while making sure that added characters are valid and string size is valid */
    while (*c >= ASCII_PRINT_START && *c <= ASCII_PRINT_END && *c != '\"') {
        /* If we've already reached the maximum string size */
        if (i >= MAX_DIRECTIVE_STRING_SIZE)
            return ERR_DIRECTIVE_STRING_CONTENT_TOO_BIG;
        /* Otherwise, add the current character to the directive entry content string*/
        (output_directive->directive_data).content_string[i] = *c;
        c++;
        i++;
    }

    /* If we've reached the line's end before closing the string */
    if (*c == '\0' || *c == '\n')
        return ERR_DIRECTIVE_STRING_NO_END_QUOTE;

        /* If we've encountered a character that is not a printable ASCII character */
    else if (*c < ASCII_PRINT_START || *c > ASCII_PRINT_END)
        return ERR_DIRECTIVE_STRING_INVALID_CHAR;

    /* Otherwise, *c is '"' and the string content was closed */
    c++;

    /* Terminate string and check its size */
    (output_directive->directive_data).content_string[i] = '\0';
    if (strlen((output_directive->directive_data).content_string) < MIN_DIRECTIVE_STRING_SIZE)
        return ERR_DIRECTIVE_STRING_CONTENT_TOO_SMALL;

    /* Go over post-content white characters */
    while (*c == ' ' || *c == '\t' || *c == '\n')
        c++;

    /* If we've reached end of line, then .string content is valid */
    if (*c == '\0')
        return OK;

    /* Otherwise, a non-white character was found */
    return ERR_DIRECTIVE_STRING_CHARS_AFTER_CONTENT;
}

return_val validate_extern_directive(const char *input_line, directive_entry *output_directive) {
    return_val rv;
    char *token;

    if (!input_line || !output_directive)
        return ERR_NULL_PTR;

    /* Set output directive entry's name */
    strcpy(output_directive->directive_name, ".extern");

    /* Get the second token of the offset line (should be the symbol name) */
    rv = get_nth_token(input_line, 2, &token);
    if (rv != OK)
        return rv;

        /* If no token after the directive keyword exists */
    else if (!token)
        return ERR_DIRECTIVE_EXTERN_MISSING_SYMBOL;

    /* Otherwise, check if it's a valid symbol name */
    rv = is_valid_symbol_name(token);
    if (rv != OK) {
        free(token);
        return rv;
    }

    /* Symbol name is valid */
    /* Set output directive entry's symbol name */
    strcpy((output_directive->directive_data).symbol_name, token);

    /* Make sure no other non-white tokens appear after directive keyword */
    free(token);
    rv = get_nth_token(input_line, 3, &token);
    if (rv != OK)
        return rv;
    /* If the token exists */
    if (token) {
        free(token);
        return ERR_DIRECTIVE_EXTERN_CHARS_AFTER_SYMBOL;
    }

    /* Otherwise, return OK */
    return OK;
}

return_val validate_entry_directive(const char *input_line, directive_entry *output_directive) {
    return_val rv;
    char *token;

    if (!input_line || !output_directive)
        return ERR_NULL_PTR;

    /* Set output directive entry's name */
    strcpy(output_directive->directive_name, ".entry");

    /* Get the second token of the offset line (should be the symbol name) */
    rv = get_nth_token(input_line, 2, &token);
    if (rv != OK)
        return rv;

        /* If no token after the directive keyword exists */
    else if (!token)
        return ERR_DIRECTIVE_ENTRY_MISSING_SYMBOL;

    /* Otherwise, check if it's a valid symbol name */
    rv = is_valid_symbol_name(token);
    if (rv != OK) {
        free(token);
        return rv;
    }

    /* Symbol name is valid */
    /* Set output directive entry's symbol name */
    strcpy((output_directive->directive_data).symbol_name, token);

    /* Make sure no other non-white tokens appear after directive keyword */
    free(token);
    rv = get_nth_token(input_line, 3, &token);
    if (rv != OK)
        return rv;
    /* If the token exists */
    if (token) {
        free(token);
        return ERR_DIRECTIVE_ENTRY_CHARS_AFTER_SYMBOL;
    }

    /* Otherwise, return OK */
    return OK;
}
