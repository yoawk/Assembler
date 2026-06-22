/**
 * macros.h
 * Header file of macros.c
 */

#ifndef MACROS_H
#define MACROS_H

#include "errors.h"

/* Macro start/end keywords */
#define MACRO_START "mcro"
#define MACRO_END "mcroend"

/* Macro entry type */
typedef struct {
    char *name; /* Macro name */
    /* Macro content (to-be expanded).
     * Set to NULL if content is empty. */
    char *macro_content;
} macro_entry;

/* Macro table type */
typedef struct {
    macro_entry **macro_entries; /* Macro entries array. NULL if empty */
    unsigned int macro_count;    /* Table's macro entries count */
} macro_table;


/**
 * Check if a given line is a valid macro definition start.
 * If no errors occur, and it's a valid macro definition start,
 * then output macro name will point to the dynamically allocated macro name (freed by caller).
 * For any other case, if no error is returned, output name will be NULL.
 * Note, that if line is an invalid macro definition start, then appropriate error
 * will be returned.
 * @param line Input line
 * @param macro_name Pointer to the output macro name string
 * @param curr_macro_table Pointer to the current macro table (for macro name collision)
 * @return OK on success.
 *          ERR_NULL_PTR on unexpected NULL pointer.
 *          ERR_MACRO_NAME_EMPTY if macro name is empty.
 *          ERR_MACRO_NAME_RESERVED if macro name collides with a reserved keyword.
 *          ERR_MACRO_NAME_DUPLICATE if macro name collides with existing macro.
 *          ERR_CHARS_AFTER_MACRO_START if extra non-white characters follow the macro name.
 */
return_val is_macro_start(const char *line, char **macro_name, const macro_table *curr_macro_table);

/**
 * Check if a given line is a valid macro definition end.
 * @param line Input line
 * @param result Pointer to the result value: 1 if given line is a valid macro definition end, 0 otherwise
 * @return OK on success. ERR_CHARS_AFTER_MACRO_END or ERR_NULL_PTR on respective errors.
 */
return_val is_macro_end(const char *line, int *result);


#endif
