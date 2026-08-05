/**
 * object_code.h
 * Header file of object_code.c
 */

#ifndef OBJECT_CODE_H
#define OBJECT_CODE_H

#include "errors.h"
#include "instructions.h"
#include "symbols.h"
#include "directives.h"

/* Initial Instruction Counter and Data Counter (of code/data images) */
#define INITIAL_IC 100
#define INITIAL_DC 0

/* Code image entry type */
typedef struct {
    unsigned short encoding;       /* Encoding of the word */
    unsigned int am_file_location; /* Line number in the .am file (same as the leading word) */
    /* Operand's addressing mode (using the bit masks in instructions.h).
     * Use INSTRUCTION_HEADER_MODE as instruction header place-holder */
    unsigned char operand_addr_mode;
    /* Optional field - only used by operand code entries that depend on a symbol */
    char symbol_name[MAX_SYMBOL_NAME_SIZE + 1]; /* Symbol name of the associated symbol */
} code_entry;

/* Data image entry type */
typedef struct {
    unsigned short encoding; /* Encoding of word */
} data_entry;


/**
 * Encodes an instruction and its (possible) operands into the code image while adding additional
 * necessary info to the relevant code entries.
 * Fully encodes the instruction's header word into the code image.
 * For any of the instruction's operands, if their addressing mode is symbol related,
 * then encoding isn't filled yet (see update_encoding for second pass),
 * but code entry's symbol name is set.
 * Assumes inputs are valid, otherwise, behavior is undefined.
 *
 * @param instruction Instruction entry to encode
 * @param operand_src Source operand to encode (may not be needed)
 * @param operand_dst Destination operand to encode (may not be needed)
 * @param code_image Code image array
 * @param IC_p Pointer to IC (will be updated)
 * @param am_file_line Instruction's line count in the .am file
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          ERR_INVALID_SRC_ADDR_MODE if source operand addressing mode is invalid.
 *          ERR_INVALID_DEST_ADDR_MODE if destination operand addressing mode is invalid.
 */
return_val encode_instruction(const instruction_entry *instruction, const operand_entry *operand_src, const operand_entry *operand_dst,
                              code_entry code_image[], unsigned short *IC_p, unsigned int am_file_line);


/**
 * Update the encoding of a code entry that depends on a
 * symbol value (operand with addressing mode of direct or relative).
 * If code entry doesn't depend on a symbol value, nothing happens.
 * @param code_image Code image array
 * @param curr_IC IC of the to-be updated code entry
 * @param symbol_table_p Pointer to symbol table
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          ERR_SYMBOL_DOES_NOT_EXIST if dependent symbol does not exist in symbol table.
 *          ERR_RELATIVE_IS_EXTERNAL if code entry is of an operand with relative addressing mode but symbol is external.
 */
return_val update_encoding(code_entry code_image[], unsigned short curr_IC, const symbol_table *symbol_table_p);

/**
 * Encodes a .data directive's value list elements into data image while updating DC accordingly.
 * Assumes directive's content is valid - otherwise behavior is undefined.
 * @param data_directive Valid data directive entry
 * @param data_image Data image array
 * @param DC_p Pointer to current DC
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 */
return_val encode_directive_data(const directive_entry *data_directive, data_entry data_image[], unsigned short *DC_p);

/**
 * Encodes a .string directive's content string into data image while updating DC accordingly.
 * Assumes directive's content is valid - otherwise behavior is undefined.
 * @param string_directive Valid string directive entry
 * @param data_image Data image array
 * @param DC_p Pointer to current DC
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 */
return_val encode_directive_string(const directive_entry *string_directive, data_entry data_image[], unsigned short *DC_p);


#endif
