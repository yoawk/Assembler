/**
 * pre_assembler.h
 * Header file of pre_assembler.c
 */

#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "errors.h"
#include "globals.h"

#include <stdio.h>


/* TODO: Choose structure for macro_entry */
typedef struct {
    char *macro_name;

} macro_entry;

/* TODO: update function documentation after function is written */
/**
 *
 *
 * Expands valid macro definitions, ignores empty lines and comment lines.
 * On error, reports error and continues analysis.
 * @param assembly_file_name
 * @return
 */
return_val pre_assembler_loop(const char *assembly_file_name);





#endif
