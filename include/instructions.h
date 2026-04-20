/**
 * instructions.h
 * Header file of instructions.c
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

/* Operand addressing modes for bit masking */
#define IMMEDIATE_MODE (1 << 0)
#define DIRECT_MODE (1 << 1)
#define RELATIVE_MODE (1 << 2)
#define REGISTER_MODE (1 << 3)

typedef struct {
    const char * const name;
    const unsigned char opcode;
    const unsigned char funct;
    const unsigned char operands_count;
    const unsigned char src_addressing;
    const unsigned char dest_addressing;
} instruction_entry;

/**
 * Returns instruction's entry by name.
 * @param inst_name Instruction name
 * @return OK on success.
 */
const instruction_entry *get_instruction(const char *inst_name);

/**
 * Checks if a given string is an instruction keyword.
 * @param input_string Given string
 * @return 1 if true, 0 if false.
 */
int is_instruction_keyword(const char *input_string);



#endif