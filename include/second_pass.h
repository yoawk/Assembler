/**
 * second_pass.h
 * Header file of second_pass.c
 */

#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "errors.h"
#include "symbols.h"
#include "object_code.h"


/**
 * Performs the second pass stage of the assembler.
 *
 * Completes the encoding of all code image entries that were left
 * unresolved in the first pass (direct and relative addressing operands).
 * Handles .entry directives from the .am file and marks the corresponding
 * symbols in the symbol table with the entry attribute.
 * If no errors are found, produces the .ob, .ent (if not empty), and
 * .ext (if not empty) output files.
 *
 * Assumes pre-assembler and first pass stages were successful.
 *
 * Clears both the symbol table and the macro table before returning,
 * regardless of whether an error occurred.
 *
 * @param assembly_file_name Input assembly file name (without the .as extension)
 * @param macro_table_p Pointer to macro table.
 * @param symbol_table_p Pointer to symbol table (partial, entry attributes are missing).
 * @param code_image Code image from first pass (partial).
 * @param data_image Data image from first pass (complete).
 * @param ICF Final IC produced by first pass.
 * @param DCF Final DC produced by first pass.
 * @return SECOND_PASS_SUCCESS on success.
 *          SECOND_PASS_FAIL on any source assembly file error detection.
 */
return_val second_pass_loop(const char *assembly_file_name, macro_table *macro_table_p, symbol_table *symbol_table_p,
                            code_entry code_image[], data_entry
                            data_image[], unsigned short ICF, unsigned short DCF);

#endif
