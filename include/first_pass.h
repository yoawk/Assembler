/**
 * first_pass.h
 * Header file of first_pass.c
 */

#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "errors.h"
#include "pre_assembler.h"
#include "object_code.h"
#include "symbols.h"


/**
 * Performs the first pass stage of the assembler.
 * Partially encodes the .am file's instructions and directives into the code image and
 * data image (symbol resolve is done in second pass).
 * Handles label definition lines and adds their symbols to the symbol table.
 * Handles the .extern directive by declaring corresponding external symbol in the table.
 * The .entry directive will be left for second pass handling.
 *
 * Assumes:
 *      .am file was created successfully and macro table created successfully.
 *      Code and data encodings will fit in code image and memory image.
 *
 *
 * On detected assembly (from .am file) error, will print error message with context.
 * If an assembly error has been found, then symbol and macro tables will be cleared.
 *
 * @param assembly_file_name Input assembly file name (same name as the .am file)
 * @param macro_table_p Pointer to macro table
 * @param symbol_table_p Pointer to symbol table (will be initialized)
 * @param code_image Code image (will be overwritten)
 * @param data_image Data image array (will be overwritten)
 * @param ICF Pointer to the output final IC (will be overwritten)
 * @param DCF Pointer to the output final DC (will be overwritten)
 * @return FIRST_PASS_SUCCESS on success. FIRST_PASS_FAIL on detected assembly error.
 */
return_val first_pass_loop(const char *assembly_file_name, macro_table *macro_table_p, symbol_table *symbol_table_p, code_entry code_image[], data_entry
                           data_image[], unsigned short *ICF, unsigned short *DCF);

#endif