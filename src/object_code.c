/**
 * object_code.c
 * Handles object file creation and memory word encoding.
 */


#include "object_code.h"

#include "globals.h"
#include "handle_file.h"
#include "utils.h"
#include "instructions.h"

#include <string.h>
#include <stdlib.h>



/* Encoding sizes as masks. Often used for readability only */
#define OPCODE_FIELD_SIZE 0xF
#define FUNCT_FIELD_SIZE 0xF
#define SRC_ADDR_MODE_FIELD_SIZE 0x3
#define DST_ADDR_MODE_FIELD_SIZE 0x3

/* Encoding offsets from the LSB */
#define OPCODE_FIELD_OFFSET 8
#define FUNCT_FIELD_OFFSET 4
#define SRC_ADDR_MODE_FIELD_OFFSET 2
#define DST_ADDR_MODE_FIELD_OFFSET 0

/* Encoding for operand addressing mode (used to convert from the bit mask value to the actual encoded value) */
#define IMMEDIATE_MODE_VAL 0
#define DIRECT_MODE_VAL 1
#define RELATIVE_MODE_VAL 2
#define REGISTER_MODE_VAL 3

/**
 * Convert bit mask value of operand addressing mode into the actual encoded value.
 * @param bit_mask Bit mask (as defined by the macros in instructions.h)
 * @return The actual encoded value
 */
unsigned short bit_mask_to_value(unsigned char bit_mask);

/**
 * Encodes an instruction's operand into the code image while adding additional
 * .am file location and operand addressing mode info to the relevant code entry.
 * If operand's addressing mode is symbol related, then encoding isn't filled yet,
 * but code entry's symbol name is set.
 * Assumes inputs are valid, otherwise, behavior is undefined.
 *
 * @param operand Operand entry to encode
 * @param code_image Code image array
 * @param IC_p Pointer to IC (will be updated)
 * @param am_file_line .am file location of the operand's instruction
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          ERR_INVALID_ADDR_MODE if operand's addressing mode is invalid.
 */
return_val encode_operand(const operand_entry *operand, code_entry code_image[], unsigned short *IC_p, unsigned int am_file_line);


return_val encode_instruction(const instruction_entry *instruction, const operand_entry *operand_src, const operand_entry *operand_dst,
                              code_entry code_image[], unsigned short *IC_p, unsigned int am_file_line) {
    return_val rv;
    unsigned short encoded_header;  /* Encoded instruction header */

    if (!instruction || !operand_src || !operand_dst || !code_image || !IC_p)
        return ERR_NULL_PTR;

    /* Encode the instruction's "header" (opcode, funct, source addressing mode, destination addressing mode)*/
    encoded_header = 0;
    encoded_header |= ((instruction->opcode & OPCODE_FIELD_SIZE) << OPCODE_FIELD_OFFSET);
    encoded_header |= ((instruction->funct & FUNCT_FIELD_SIZE) << FUNCT_FIELD_OFFSET);

    /* If the instruction has a source operand */
    if (instruction->operands_count >= 2)
        encoded_header |= ((bit_mask_to_value(operand_src->addressing_mode) & SRC_ADDR_MODE_FIELD_SIZE) << SRC_ADDR_MODE_FIELD_OFFSET);
    /* Otherwise, field should be left zeroed out */

    /* If the instruction has a destination operand */
    if (instruction->operands_count >= 1)
        encoded_header |= ((bit_mask_to_value(operand_dst->addressing_mode) & DST_ADDR_MODE_FIELD_SIZE) << DST_ADDR_MODE_FIELD_OFFSET);
    /* Otherwise, field should be left zeroed out */

    /* Store header encoding, set operand addressing mode to the place-holder value,
     * and set .am file location in the code image. */
    code_image[*IC_p].encoding = (encoded_header & WORD_MASK);
    code_image[*IC_p].am_file_location = am_file_line;
    code_image[*IC_p].operand_addr_mode = INSTRUCTION_HEADER_MODE;

    /* Header encoding is finished, and IC counter can advance */
    (*IC_p)++;

    /* If instruction has 2 operands, encode both source operand and destination operand */
    if (instruction->operands_count >= 2) {
        /* Encode the source operand: (when possible at this current stage) set code entry's encoding,
         * ARE attribute, .am file location (same as the instruction header), and symbol name (if needed).
         * IC is advanced. */
        rv = encode_operand(operand_src, code_image, IC_p, am_file_line);
        /* Add additional error context if needed */
        if (rv == ERR_INVALID_ADDR_MODE)
            return ERR_INVALID_SRC_ADDR_MODE;
        else if (rv != OK)
            return rv;

        /* Repeat for the destination operand */
        rv = encode_operand(operand_dst, code_image, IC_p, am_file_line);
        /* Add additional error context if needed */
        if (rv == ERR_INVALID_ADDR_MODE)
            return ERR_INVALID_DEST_ADDR_MODE;
        else if (rv != OK)
            return rv;
    }
    /* If instruction has only 1 operand, only desitination operand will be encoded */
    else if (instruction->operands_count == 1) {
        /* Encode the destination operand: (when possible at this current stage) set code entry's encoding,
         * ARE attribute, .am file location (same as the instruction header), and symbol name (if needed).
         * IC is advanced. */
        rv = encode_operand(operand_dst, code_image, IC_p, am_file_line);
        /* Add additional error context if needed */
        if (rv == ERR_INVALID_ADDR_MODE)
            return ERR_INVALID_DEST_ADDR_MODE;
        else if (rv != OK)
            return rv;
    }
    /* Otherwise, no additional encoding is needed */

    return OK;
}

return_val encode_operand(const operand_entry *operand, code_entry code_image[], unsigned short *IC_p, unsigned int am_file_line) {
    if (!operand || !code_image || !IC_p)
        return ERR_NULL_PTR;


    if (operand->addressing_mode == IMMEDIATE_MODE) {
        /* Set encoding value to be the immediate value stored as word by the operand entry */
        code_image[*IC_p].encoding = (operand->value & WORD_MASK);
    }
    else if (operand->addressing_mode == DIRECT_MODE || operand->addressing_mode == RELATIVE_MODE) {
        /* Set symbol name, and leave encoding empty (will be filled in seconds pass) */
        code_image[*IC_p].encoding = 0;
        strcpy(code_image[*IC_p].symbol_name, operand->label_str);
    }
    else if (operand->addressing_mode == REGISTER_MODE) {
        /* Set encoding (operand value is the register number) */
        code_image[*IC_p].encoding = ((1 << operand->value) & WORD_MASK);
    }

    else
        return ERR_INVALID_ADDR_MODE;

    /* Set .am file line location */
    code_image[*IC_p].am_file_location = am_file_line;
    /* Set operand addressing mode */
    code_image[*IC_p].operand_addr_mode = operand->addressing_mode;

    /* Advance IC */
    (*IC_p)++;

    return OK;
}

unsigned short bit_mask_to_value(unsigned char bit_mask) {
    switch (bit_mask) {
        case IMMEDIATE_MODE:
            return IMMEDIATE_MODE_VAL;
        case DIRECT_MODE:
            return DIRECT_MODE_VAL;
        case RELATIVE_MODE:
            return RELATIVE_MODE_VAL;
        case REGISTER_MODE:
            return REGISTER_MODE_VAL;
        default:
            return 0;
    }
}

return_val encode_directive_data(const directive_entry *data_directive, data_entry data_image[], unsigned short *DC_p) {
    size_t i;

    if (!data_directive || !data_image || !DC_p)
        return ERR_NULL_PTR;

    /* Go over .data directive's value list elements and encode each element */
    for (i = 0; i < (data_directive->directive_data).data_integers.integer_count; i++) {
        data_image[*DC_p].encoding = ((data_directive->directive_data).data_integers.integer_list[i] & WORD_MASK);
        (*DC_p)++;
    }

    return OK;
}

return_val encode_directive_string(const directive_entry *string_directive, data_entry data_image[], unsigned short *DC_p) {
    size_t i;

    if (!string_directive || !data_image || !DC_p)
        return ERR_NULL_PTR;

    /* Go over .string directive's content string and encode each ASCII character including the terminating '\0' */
    for (i = 0; i <= strlen((string_directive->directive_data).content_string); i++) {
        data_image[*DC_p].encoding = (string_directive->directive_data).content_string[i];
        (*DC_p)++;
    }

    return OK;
}

