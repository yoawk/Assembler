/**
 * instructions.h
 * Header file of instructions.c
 * Includes definition of instruction entry
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "errors.h"

#include "symbols.h"

/* Immediate value is stored as two's complement,
 * so minimum value is -2^(word_size - 1) and maximum is 2^(word_size -1) - 1 */
#define MIN_IMMEDIATE_VAL ( -((1 << (WORD_SIZE - 1)) ))
#define MAX_IMMEDIATE_VAL ( ( 1 << (WORD_SIZE - 1) ) - 1 )


/* Operand addressing modes for bit masking */
#define INSTRUCTION_HEADER_MODE 0    /* Used as place-holder for instruction header's code entry's operand mode */
#define IMMEDIATE_MODE (1 << 0) /* Operand is an immediate value, e.g. #-1 */
#define DIRECT_MODE (1 << 1)    /* Operand is a label pointing to an address, e.g. my_var */
#define RELATIVE_MODE (1 << 2)  /* Operand is a label pointing to an address (used for branching), e.g. %loop */
#define REGISTER_MODE (1 << 3)  /* Operand is a register, e.g. r2 */

/* Operand addressing modes prefix characters */
#define IMMEDIATE_PREFIX_CHAR '#'
#define RELATIVE_PREFIX_CHAR '%'

/**
 * Instruction entry structure.
 * Addressing modes should be set using the corresponding bit masking,
 * or set to 0 if operand is not used.
 *
 * First word of an instruction (instruction header word) will be encoded as:
 * +----+----+---+---+---+---+---+---+-----+-----+-----+-----+
 * | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 |  3  |  2  |  1  |  0  |
 * +----+----+---+---+---+---+---+---+-----+-----+-----+-----+
 * |    opcode       |      funct    |src. addr. | dst. addr.|
 * +----+----+---+---+---+---+---+---+-----+-----+-----+-----+
 */
typedef struct {
    const char *const name;
    const unsigned char opcode;
    const unsigned char funct;
    const unsigned char operands_count;  /* 0-2 */
    const unsigned char src_addressing;  /* Addressing mode for first operand (if exists) */
    const unsigned char dest_addressing; /* Addressing mode for second operand (if exists) */
} instruction_entry;

typedef struct {
    unsigned char addressing_mode;            /* The respective bit mask (of only one mode) */
    short value;                              /* For immediate or register modes, immediate value or register number */
    char label_str[MAX_SYMBOL_NAME_SIZE + 1]; /* For direct or relative mode, the label name */
} operand_entry;

/**
 * Returns an instruction's entry by name.
 * @param inst_name Instruction name
 * @return If instruction exists, returns pointer to the instruction's entry (which shouldn't be freed)
 *          If instruction name doesn't match an existing instruction or inst_name is NULL, returns NULL.
 */
const instruction_entry *get_instruction(const char *inst_name);

/**
 * Checks if a given string is an instruction name/keyword.
 * @param input_string Given string
 * @return 1 if true, 0 if false and 0 for NULL pointer.
 */
int is_instruction_keyword(const char *input_string);


/**
 * Checks if a given line is a valid instruction line.
 * If no errors occur and line is a valid instruction line, the pointer to the output instruction
 * entry will point to the corresponding instruction entry (which shouldn't be freed),
 * and the relevant output operand entries will be set accordingly in respect to the operand count.
 * If no error occurs but line is not an instruction line, output instruction pointer will be NULL.
 * Otherwise, outputs are undefined.
 *
 * @param line Given line. Undefined behavior if not a valid string.
 * @param label_flag 1 if a label definition is present in the line, 0 otherwise.
 * @param output_instruction Pointer to output instruction entry
 * @param output_operand_src Pointer to the output source operand entry (might not be needed)
 * @param output_operand_dst Pointer to the output destination operand entry (might not be needed)
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          Comma related errors of the operands (see count_parameters for full list),
 *          ERR_MISSING_OPERANDS, ERR_TOO_MANY_OPERANDS for the respective errors.
 *          ERR_INVALID_SRC_ADDR_MODE for addressing mode mismatch of instruction and source operand.
 *          ERR_INVALID_DEST_ADDR_MODE, similarly, for destination operand.
 */
return_val is_valid_instruction_line(const char *line, int label_flag, const instruction_entry **output_instruction,
                                     operand_entry *output_operand_src, operand_entry *
                                     output_operand_dst);

/**
 * Analyzes instruction operand string and returns corresponding operand entry
 * If operand string is a valid operand of some addressing mode, on success,
 * will set the output_operand values to those of the operand string.
 *
 * @param operand_str Operand string (without leading/trailing white characters)
 * @param output_operand Output operand entry.
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          ERR_IMMEDIATE_EMPTY for an empty immediate operand,
 *          ERR_IMMEDIATE_UNKNOWN for general invalid immediate value,
 *          ERR_IMMEDIATE_OUT_OF_RANGE for an immediate out of range,
 *          ERR_RELATIVE_EMPTY for an empty relative operand,
 *          ERR_EMPTY_OPERAND for an empty operand.
 *          ERR_SYMBOL_NAME_TOO_BIG if symbol name exceeds MAX_SYMBOL_NAME_SIZE.
 *          ERR_SYMBOL_NAME_RESERVED if symbol name is a reserved keyword (instruction/directive/register).
 *          ERR_SYMBOL_FIRST_CHAR if symbol name doesn't start with a letter.
 *          ERR_SYMBOL_NON_ALPHANUMERIC if some other character is not alphanumeric.
 */
return_val get_operand(const char *operand_str, operand_entry *output_operand);

#endif
