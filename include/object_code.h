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



#endif
