/**
 * pre_assembler.h
 * Header file of pre_assembler.c
 */

#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "errors.h"
#include "macros.h"


/**
 * The main loop of the pre-assembler stage.
 * Operates on a given .as file and performs macro expansion, comment/white lines filtering
 * and basic error detection (mainly macro related).
 * On success, result assembly code will be written into a new .am file with a corresponding name.
 * When an internal error occurs, or when an error is found in the .as file,
 * reports error and, if possible, continues processing the .as file.
 * The resulting .am file will not be created if any error occurs, or is found, throughout the function.
 *
 * Assumes there are no nested macro definitions.
 * Assumes each macro definition start has a corresponding macro ending.
 * Assumes macro calls appear only after corresponding macro definitions.
 *
 * @param assembly_file_name The file name of the input .as file (and, if produced, .am file) without the .as extension.
 * @param macro_table_p Pointer to an empty macro table that will be overridden and filled with macro entries.
 * @return PRE_ASSEMBLER_SUCCESS on success. PRE_ASSEMBLER_FAIL on detected assembly error.
 */
return_val pre_assembler_loop(const char *assembly_file_name, macro_table *macro_table_p);

#endif
