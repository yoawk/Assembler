/**
 * symbols.h
 * Header file of symbols.c
 */

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "pre_assembler.h"


#define MAX_SYMBOL_NAME_SIZE 31 /* Excluding ':' (the label end character) and '\0' */
#define LABEL_END_CHAR ':'  /* The ending character of a label definition */


/* Bit masks for symbol attributes */
#define SYMBOL_CODE_ATTR (1 << 0)
#define SYMBOL_DATA_ATTR (1 << 1)
#define SYMBOL_EXTERN_ATTR (1 << 2)
#define SYMBOL_ENTRY_ATTR (1 << 3)

/* External symbol location value */
#define SYMBOL_EXTERN_LOCATION 0

/* Symbol entry type */
typedef struct {
    char name[MAX_SYMBOL_NAME_SIZE + 1]; /* One more char for null character */
    unsigned short location;             /* Location in output binary (SYMBOL_EXTERN_LOCATION for extern) */
    unsigned char attributes;            /* Bit mask of the symbol's attributes (code/data, extern/entry) */
} symbol_entry;

/* Symbol table type */
typedef struct {
    symbol_entry **symbol_entries;
    unsigned int symbol_count;
} symbol_table;


/**
 * Check if a line starts with a valid label/symbol definition.
 * A valid symbol definition line starts with a symbol name of at most MAX_SYMBOL_NAME_SIZE of
 * alphanumeric characters where the first character is alphabetical.
 * Symbol name also can't be a macro name, a reserved keyword or a previously defined symbol name.
 * Immediately after the symbol name, a LABEL_END_CHAR should appear.
 *
 * If line contains a valid label definition and no errors occurred, the label's symbol name
 * will be dynamically allocated and returned (freed by caller).
 * If line doesn't contain a label definition and no errors occurred, then output symbol name will be NULL.
 * @param line Given line
 * @param symbol_table_p Pointer to symbol table (for symbol/symbol name collision)
 * @param macro_table_p Pointer to macro table (for macro/symbol name collision)
 * @param output_symbol_name Output symbol name
 * @return OK on success. ERR_NULL_PTR on input NULL pointer.
 *          ERR_SYMBOL_NAME_TOO_BIG if symbol name exceeds MAX_SYMBOL_NAME_SIZE.
 *          ERR_SYMBOL_NAME_EMPTY if symbol name is empty.
 *          ERR_SYMBOL_NAME_RESERVED if symbol name is a reserved keyword (instruction/directive/register).
 *          ERR_SYMBOL_FIRST_CHAR if symbol name doesn't start with a letter.
 *          ERR_SYMBOL_NON_ALPHANUMERIC if some other character is not alphanumeric.
 *          ERR_SYMBOL_IS_MACRO_NAME if symbol name is a previously defined macro's name.
 *          ERR_SYMBOL_LABEL_ALREADY_EXISTS if symbol name collides with an already defined symbol name.
 */
return_val is_label_definition(const char *line, const symbol_table *symbol_table_p, const macro_table *macro_table_p,
                              char **output_symbol_name);

/**
 * Validates a symbol name.
 *
 * A valid symbol name:
 *      is non-empty,
 *     does not exceed MAX_SYMBOL_NAME_SIZE characters,
 *      begins with an alphabetic character (upper/lower case),
 *      contains only alphanumeric characters,
 *      is not a reserved keyword.
 *
 * The function does not check for collisions with existing symbols or macros.
 *
 * @param symbol_name Input symbol name
 * @return OK if symbol name is valid.
 *          ERR_NULL_PTR if symbol name is NULL.
 *          ERR_SYMBOL_NAME_TOO_BIG if symbol name exceeds MAX_SYMBOL_NAME_SIZE.
 *          ERR_SYMBOL_NAME_EMPTY if symbol name is empty.
 *          ERR_SYMBOL_NAME_RESERVED if symbol name is a reserved keyword (instruction/directive/register).
 *          ERR_SYMBOL_FIRST_CHAR if symbol name doesn't start with a letter.
 *          ERR_SYMBOL_NON_ALPHANUMERIC if some other character is not alphanumeric.
 *
 */
return_val is_valid_symbol_name(const char *symbol_name);

/**
 * Checks if a string is a symbol name of an entry in a symbol table
 * and if so, returns the symbol entry (shouldn't be freed).
 * Behavior is undefined if symbol table is not valid.
 * @param input_string Input string
 * @param symbol_table_p Given symbol table pointer
 * @return If string matches with a symbol's name, corresponding symbol entry will be returned.
 *          Otherwise, if there are no matches, or if a NULL pointer was passed, returns NULL.
 */
symbol_entry *is_in_symbol_table(const char *input_string, const symbol_table *symbol_table_p);

/**
 * Initialize symbol table as an empty table (symbol entries array will be NULL and symbol count will be 0).
 * Assumes symbol table doesn't contain allocated entries.
 * @param symbol_table_p Pointer to the to-be initalized symbol table.
 * @return OK on success. ERR_NULL_PTR if passed pointer is NULL.
 */
return_val init_symbol_table(symbol_table *symbol_table_p);

/**
 * Clears a given symbol table, freeing its entries and resetting its values.
 * Behavior is undefined if symbol table or its components are invalid.
 * @param symbol_table_p Given symbol table
 * @return OK on success. ERR_NULL_PTR on NULL pointer.
 */
return_val clear_symbol_table(symbol_table *symbol_table_p);

/**
 * Allocates and adds a specified symbol entry to a given symbol table.
 * Symbol table can be empty, in which case, symbol entries array will be initialized
 * with dynamic memory allocation.
 * On any error, symbol entry won't be added to symbol table.
 * Behavior is undefined if symbol table or specified symbol entry fields are invalid.
 *
 * @param symbol_table_p Pointer to symbol table
 * @param symbol_name Specified symbol entry name (non-empty with max size of MAX_SYMBOL_NAME_SIZE)
 * @param symbol_location Specified symbol location value
 * @param symbol_attributes Specified symbol attributes bit mask
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 */
return_val add_symbol_to_table(symbol_table *symbol_table_p, const char *symbol_name, unsigned short symbol_location, unsigned char
                               symbol_attributes);


#endif
