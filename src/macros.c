/**
 * macros.c
 * Handles assembly macro definition lines, macro entries and macro tables.
 */

#include "macros.h"

#include "handle_file.h"
#include "keywords.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

return_val is_macro_start(const char *line, char **macro_name, const macro_table *curr_macro_table) {
    return_val rv;
    unsigned int i;
    char *token_string; /* Used for storing a non-white character sequence of line */

    if (!line || !macro_name || !curr_macro_table)
        return ERR_NULL_PTR;

    /* Initialize output to avoid function misuse of the caller */
    *macro_name = NULL;

    /* Get the first sequence of line */
    rv = get_nth_token(line, 1, &token_string);
    if (rv != OK)
        return rv;

    /* If no such sequence was found, then it's not a macro start */
    if (!token_string) {
        return OK;
    }

    /* If a sequence was found, but it's not a macro start string */
    else if (strcmp(token_string, MACRO_START) != 0) {
        free(token_string);
        return OK;
    }

    /* Otherwise, the first sequence is the macro start string */
    free(token_string);

    /* Get next sequence of line to check macro name */
    rv = get_nth_token(line, 2, &token_string);
    if (rv != OK)
        return rv;

    /* If no such sequence was found */
    if (!token_string)
        return ERR_MACRO_NAME_EMPTY;

    /* Otherwise, a macro name is present */


    /* Check if macro name is invalid */
    /* Note, collision with label names will be checked in first pass */

    /* Check if name is a reserved keyword  */
    if (is_reserved_keyword(token_string)) {
        free(token_string);
        return ERR_MACRO_NAME_RESERVED;
    }

    /* Go over previously defined macros and check for name collision. */
    for (i = 0; i < curr_macro_table->macro_count; i++) {
        if (strcmp(token_string, (curr_macro_table->macro_entries)[i]->name) == 0) {
            free(token_string);
            return ERR_MACRO_NAME_DUPLICATE;
        }
    }

    /* Otherwise, macro name is valid */
    *macro_name = token_string;

    /* Check if no other non-empty sequences appear after the macro definition start */
    rv = get_nth_token(line, 3, &token_string);
    if (rv != OK) {
        free(*macro_name);
        *macro_name = NULL;
        return rv;
    }

    /* If a sequence of non-white characters appears after the macro name,
     * then macro definition start is invalid */
    else if (token_string != NULL) {
        free(token_string);
        free(*macro_name);
        *macro_name = NULL;
        return ERR_CHARS_AFTER_MACRO_START;
    }

    /* Otherwise, macro definition start is valid (and output macro name is set) */
    return OK;
}

return_val is_macro_end(const char *line, int *result) {
    return_val rv;
    char *token_string; /* Used to hold non-white character sequences of line */

    if (!line || !result)
        return ERR_NULL_PTR;

    /* Initialize result value to 0 to avoid function misuse of the caller */
    *result = 0;

    /* Get the first sequence of line */
    rv = get_nth_token(line, 1, &token_string);
    if (rv != OK)
        return rv;

    /* If no such sequence was found, then line is not a valid macro definition end */
    if (!token_string) {
        *result = 0;
        return OK;
    }

    /* If a sequence was found, but it's not a macro end string */
    else if (strcmp(token_string, MACRO_END) != 0) {
        *result = 0;
        free(token_string);
        return OK;
    }

    /* Otherwise, the first sequence is the macro end string */

    free(token_string);

    /* Check if no other non-empty sequences appear after the macro definition end string */
    rv = get_nth_token(line, 2, &token_string);
    if (rv != OK)
        return rv;

    if (token_string != NULL) {
        free(token_string);
        return ERR_CHARS_AFTER_MACRO_END;
    }

    /* Otherwise, macro definition end is valid */
    *result = 1;
    return OK;
}

