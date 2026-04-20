/**
 * instructions.h
 * Header file of instructions.c
 * Includes definition of instruction entry
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

/* Operand addressing modes for bit masking */
#define IMMEDIATE_MODE (1 << 0) /* Operand is an immediate value, e.g. #-1 */
#define DIRECT_MODE (1 << 1)    /* Operand is a label pointing to an address, e.g. my_var */
#define RELATIVE_MODE (1 << 2)  /* Operand is a label pointing to an address (used for branching), e.g. %loop */
#define REGISTER_MODE (1 << 3)  /* Operand is a register, e.g. r2 */

/**
 * Instruction entry structure.
 * Addressing modes should be set using the corresponding bit masking,
 * or set to 0 if operand is not used.
 *
 * First word of an instruction will be encoded as:
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
    const unsigned char operands_count;
    const unsigned char src_addressing;  /* Addressing mode for first operand (if exists) */
    const unsigned char dest_addressing; /* Addressing mode for second operand (if exists) */
} instruction_entry;

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


#endif
