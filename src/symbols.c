/**
 * symbols.c
 * Handles symbol entries and symbol tables
 */

#include "symbols.h"

#include "handle_file.h"
#include "keywords.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


return_val is_label_definition(const char *line, const symbol_table *symbol_table_p, const macro_table *macro_table_p,
                              char **output_symbol_name) {
    return_val rv;
    char *token;
    size_t label_size;

    if (!line || !macro_table_p || !symbol_table_p || !output_symbol_name)
        return ERR_NULL_PTR;

    /* Initialize output to avoid caller misuse */
    *output_symbol_name = NULL;

    /* Get the first token of the line (non-white character sequence) */
    rv = get_nth_token(line, 1, &token);
    if (rv != OK)
        return rv;
        /* If the line doesn't contain any token, then there is no label */
    else if (!token) {
        return OK;
    }

    /* Otherwise, check if the first token is a valid label */

    /* Check if token end with the label end character */
    label_size = strlen(token); /* Will include the label end character */
    if (token[label_size - 1] != LABEL_END_CHAR) {
        free(token);
        return OK;
    }

    /* Update label size to exclude the ending character */
    label_size--;

    /* Extract label/symbol name (without end character) into output symbol name */
    /* output symbol name will be freed by caller */
    *output_symbol_name = (char *) malloc_helper(label_size + 1);

    /* Copy and add null character */
    memcpy(*output_symbol_name, token, label_size);
    (*output_symbol_name)[label_size] = '\0';
    free(token);

    /* Check if symbol name is valid */
    rv = is_valid_symbol_name(*output_symbol_name);
    if (rv != OK) {
        free(*output_symbol_name);
        *output_symbol_name = NULL;
        return rv;
    }

    /* Check if symbol name collides with a previously defined macro's name */
    if (is_in_macro_table(*output_symbol_name, macro_table_p)) {
        free(*output_symbol_name);
        *output_symbol_name = NULL;
        return ERR_SYMBOL_IS_MACRO_NAME;
    }

    /* Check if symbol name collides with a prevously defined symbol's name */
    if (is_in_symbol_table(*output_symbol_name, symbol_table_p)) {
        free(*output_symbol_name);
        *output_symbol_name = NULL;
        return ERR_SYMBOL_LABEL_ALREADY_EXISTS;
    }


    /* Otherwise, symbol name is valid and the line starts with a valid label */
    return OK;
}


return_val is_valid_symbol_name(const char *symbol_name) {
    size_t i;

    if (!symbol_name)
        return ERR_NULL_PTR;

    /* Check if smybol name size is valid */
    if (strlen(symbol_name) > MAX_SYMBOL_NAME_SIZE)
        return ERR_SYMBOL_NAME_TOO_BIG;

    /* Check if symbol is empty */
    if (strlen(symbol_name) == 0) {
        return ERR_SYMBOL_NAME_EMPTY;
    }

    /* Check if symbol name is a reserved keyword */
    if (is_reserved_keyword(symbol_name))
        return ERR_SYMBOL_NAME_RESERVED;


    /* Check if symbol name starts with a letter */
    if (!isalpha(symbol_name[0]))
        return ERR_SYMBOL_FIRST_CHAR;


    /* Check if symbol name is only alphanumerical */
    for (i = 1; i < strlen(symbol_name); i++) {
        if (!isalnum(symbol_name[i]))
            return ERR_SYMBOL_NON_ALPHANUMERIC;
    }

    /* Otherwise, symbol name is valid */
    return OK;
}


symbol_entry *is_in_symbol_table(const char *input_string, const symbol_table *symbol_table_p) {
    unsigned int i;

    if (!input_string || !symbol_table_p)
        return NULL;

    /* Go over symbol table entries and comapre their names with input string */
    for (i = 0; i < symbol_table_p->symbol_count; i++) {
        if (strcmp(input_string, (symbol_table_p->symbol_entries)[i]->name) == 0)
            return (symbol_table_p->symbol_entries)[i];
    }

    return NULL;
}

return_val init_symbol_table(symbol_table *symbol_table_p) {
    if (!symbol_table_p)
        return ERR_NULL_PTR;

    symbol_table_p->symbol_entries = NULL;
    symbol_table_p->symbol_count = 0;

    return OK;
}

return_val clear_symbol_table(symbol_table *symbol_table_p) {
    unsigned int i;
    if (!symbol_table_p)
        return ERR_NULL_PTR;

    /* Free allcoated memory of entries and entries array */
    if (symbol_table_p->symbol_entries) {
        for (i = 0; i < symbol_table_p->symbol_count; i++)
            free((symbol_table_p->symbol_entries)[i]);
        free(symbol_table_p->symbol_entries);
    }
    symbol_table_p->symbol_entries = NULL;
    symbol_table_p->symbol_count = 0;

    return OK;
}

return_val add_symbol_to_table(symbol_table *symbol_table_p, const char *symbol_name, unsigned short symbol_location,
                               unsigned char symbol_attributes) {
    symbol_entry *entry_to_add;

    if (!symbol_table_p || !symbol_name)
        return ERR_NULL_PTR;

    /* Allocate symbol entry with specified values */
    entry_to_add = (symbol_entry *) malloc_helper(sizeof(symbol_entry));

    strcpy(entry_to_add->name, symbol_name);
    entry_to_add->location = symbol_location;
    entry_to_add->attributes = symbol_attributes;


    /* If symbol table is empty, first allocate memory for the symbol entries array */
    /* We assume that if symbol count is 0, then symbol entries array is not initialized yet */
    if (symbol_table_p->symbol_count == 0) {
        /* Allocate memory to fit the symbol entry */
        symbol_table_p->symbol_entries = (symbol_entry **) malloc_helper(sizeof(symbol_entry *));
    }
    /* Otherwise, extend the symbol entries array to fit the additional entry */
    else {
        /* Expand memory chunk by one symbol entry pointer  */
        symbol_table_p->symbol_entries = (symbol_entry **) realloc_helper(symbol_table_p->symbol_entries,
                                                        sizeof(symbol_entry *) * (symbol_table_p->symbol_count + 1));
    }

    /* Add symbol entry and increment symbol count */
    (symbol_table_p->symbol_entries)[symbol_table_p->symbol_count] = entry_to_add;
    (symbol_table_p->symbol_count)++;

    return OK;
}
