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


/* Object file's field sizes */
#define OBJECT_FILE_FIELD_ADDRESS_SIZE 4
#define OBJECT_FILE_FIELD_CONTENT_SIZE 3

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

/**
 * Returns the digit count of an unsigned short.
 * @param input_num Input unsigned short
 * @return The number's digit count
 */
int count_digits(unsigned short input_num);

/**
 * Writes given memory word info to object file using the format:
 *          (WORD'S MEMORY ADDRESS)(SPACE)(WORD CONTENT)(SPACE)(WORD'S ARE CHARACTER)
 * Where the memory address is OBJECT_FILE_FIELD_ADDRESS_SIZE digits in decimal with padding zeros,
 * and the word's content is OBJECT_FILE_FIELD_CONTENT_SIZE digits of hex.
 *
 * @param memory_address Memory word's address in memory
 * @param word_encoding Memory word's encoding
 * @param ARE_char Memory word's ARE character
 * @param ob_fp Pointer to open object file for writing
 * @return OK on success. ERR_NULL_PTR on unexpected NULL pointer.
 *          ERR_FAILED_TO_WRITE_OB if write operation to object file failed.
 */
return_val write_object_file_line(unsigned short memory_address, unsigned short word_encoding, char ARE_char, FILE *ob_fp);


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

return_val update_encoding(code_entry code_image[], unsigned short curr_IC, const symbol_table *symbol_table_p) {
    code_entry curr_entry;
    symbol_entry *temp_symbol_entry;

    if (!code_image || !symbol_table_p)
        return ERR_NULL_PTR;

    curr_entry = code_image[curr_IC];

    /* Check if current code image entry is an operand that depends on a symbol value */
    /* If current entry is an operand with direct addressing mode */
    if (curr_entry.operand_addr_mode == DIRECT_MODE) {
        /* Check if current entry's associated symbol is present is symbol table */
        temp_symbol_entry = is_in_symbol_table(curr_entry.symbol_name, symbol_table_p);
        /* If symbol doesn't exist */
        if (!temp_symbol_entry)
            return ERR_SYMBOL_DOES_NOT_EXIST;

        /* Otherwise, set updated encoding to the matched symbol's value */
        code_image[curr_IC].encoding = (temp_symbol_entry->location & WORD_MASK);
        return OK;
    }

    /* If current entry is an operand with relative addressing mode */
    else if (curr_entry.operand_addr_mode == RELATIVE_MODE) {
        /* Check if current entry's associated symbol is present is symbol table */
        temp_symbol_entry = is_in_symbol_table(curr_entry.symbol_name, symbol_table_p);
        /* If symbol doesn't exist */
        if (!temp_symbol_entry)
            return ERR_SYMBOL_DOES_NOT_EXIST;

            /* If symbol is external */
        else if ((temp_symbol_entry->attributes & SYMBOL_EXTERN_ATTR) != 0)
            return ERR_RELATIVE_IS_EXTERNAL;

        /* Otherwise, set updated encoding to the offset of the operand and symbol */
        code_image[curr_IC].encoding = ((temp_symbol_entry->location - curr_IC) & WORD_MASK);
        return OK;
    }

    /* Otherwise, code entry doesn't need encoding update */
    return OK;
}


return_val produce_object_file(const char *assembly_file_name, const code_entry code_image[], const data_entry data_image[],
                               unsigned short ICF, unsigned short DCF, const symbol_table *symbol_table_p) {
    return_val rv;
    unsigned short curr_address;

    FILE *ob_fp;    /* .ob file's file pointer */
    size_t write_size, written_size;
    char *write_buffer;

    code_entry curr_code_entry;
    data_entry curr_data_entry;
    char ARE_char;  /* ARE character for file writing */
    const symbol_entry *temp_symbol_entry;

    if (!assembly_file_name || !code_image || !data_image || !symbol_table_p)
        return ERR_NULL_PTR;

    /* Open the .ob file for writing */
    rv = fopen_helper(assembly_file_name, OBJECT_FILE_EXTENSION, "w", &ob_fp);
    /* Provide additional context */
    if (rv == ERR_FAILED_TO_OPEN_FILE)
        return ERR_FAILED_TO_OPEN_OB_FOR_WRITE;
    else if (rv != OK)
        return rv;

    /* Write the object file's header line */
    /* Its format is:
     * (CODE IMAGE SIZE IN DECIMAL)(SPACE)(DATA IMAGE SIZE IN DECIMAL) */

    /* Calculate the appropriate write size according to the format */
    write_size = count_digits(ICF - INITIAL_IC) + 1 + count_digits(DCF - INITIAL_DC);    /* The sum of digits + space */
    /* Setup string buffer */
    write_buffer = malloc_helper(write_size + 1); /* Added 1 byte for '\0' */
    sprintf(write_buffer, "%hu %hu", ICF - INITIAL_IC, DCF - INITIAL_DC);

    /* Perform write */
    written_size = fwrite(write_buffer, sizeof(char), write_size, ob_fp);
    free(write_buffer);
    /* If write failed */
    if (written_size < write_size)
        return ERR_FAILED_TO_WRITE_OB;

    /* File header written successfully */



    /* the format of the rest of the .ob file is:
     * (WORD'S MEMORY ADDRESS)(SPACE)(WORD CONTENT)(SPACE)(WORD'S ARE CHARACTER)
     * Where the memory address is OBJECT_FILE_FIELD_ADDRESS_SIZE digits in decimal with padding zeros,
     * and the word's content is OBJECT_FILE_FIELD_CONTENT_SIZE digits of hex. */


    /* Write code image to .ob file */
    for (curr_address = INITIAL_IC; curr_address < ICF; curr_address++) {
        curr_code_entry = code_image[curr_address];

        /* Set ARE character */
        switch (curr_code_entry.operand_addr_mode) {
            case INSTRUCTION_HEADER_MODE:
                ARE_char = INSTRUCTION_HEADER_ARE;
                break;
            case IMMEDIATE_MODE:
                ARE_char = IMMEDIATE_MODE_ARE;
                break;
            case RELATIVE_MODE:
                ARE_char = RELATIVE_MODE_ARE;
                break;
            case REGISTER_MODE:
                ARE_char = REGISTER_MODE_ARE;
                break;
            case DIRECT_MODE:
                /* An operand word with a direct addressing mode can be either
                 * Relocatable or External - depending on the associated symbol */

                /* Find its symbol */
                temp_symbol_entry = is_in_symbol_table(curr_code_entry.symbol_name, symbol_table_p);
                /* Code image entries are assumed to be valid */
                if (!temp_symbol_entry)
                    return ERR_NULL_PTR;

                /* If associated symbol is external, set ARE value to External */
                if ((temp_symbol_entry->attributes & SYMBOL_EXTERN_ATTR) != 0)
                    ARE_char = 'E';
                /* Otherwise, set it to Relocatable */
                else
                    ARE_char = 'R';

                break;

            default:
                /* Code image is assumed to be valid */
                return ERR_UNEXPECTED_VALUE;
        }


        /* Write word info into object file */
        rv = write_object_file_line(curr_address, curr_code_entry.encoding, ARE_char, ob_fp);
        if (rv != OK)
            return rv;

    }

    /* Write data image to .ob file */
    /* Note that the data image comes right after the code image, so actual data addresses should be offset by ICF */
    for (curr_address = INITIAL_DC; curr_address < DCF; curr_address++) {
        curr_data_entry = data_image[curr_address];
        ARE_char = 'A'; /* Data is absolute */

        /* Write word info into object file */
        rv = write_object_file_line(curr_address + ICF, curr_data_entry.encoding, ARE_char, ob_fp);
        if (rv != OK)
            return rv;
    }

    /* Close .ob file */
    if (fclose(ob_fp) == EOF)
        return ERR_FAILED_TO_CLOSE_OB;

    /* .ob file can't be empty because of its header, and therefore not 'empty' check is needed */

    return OK;
}

int count_digits(unsigned short input_num) {
    int result = 0;

    if (input_num == 0)
        return 1;

    while (input_num != 0) {
        input_num /= 10;
        result++;
    }

    return result;
}

return_val write_object_file_line(unsigned short memory_address, unsigned short word_encoding, char ARE_char, FILE *ob_fp) {
    size_t write_size, written_size;
    char *write_buffer;

    if (!ob_fp)
        return ERR_NULL_PTR;


    /* Calculate the appropriate write size according to the format (with added new line to separate lines) */
    /*        newline         word address          space           word content         space ARE */
    write_size = 1 + OBJECT_FILE_FIELD_ADDRESS_SIZE + 1 + OBJECT_FILE_FIELD_CONTENT_SIZE + 1 + 1;


    /* Setup string buffer */
    write_buffer = malloc_helper(write_size + 1); /* Added 1 byte for '\0' */
    sprintf(write_buffer, "\n%0*hu %0*hX %c", OBJECT_FILE_FIELD_ADDRESS_SIZE, memory_address, OBJECT_FILE_FIELD_CONTENT_SIZE, word_encoding, ARE_char);

    /* Perform write */
    written_size = fwrite(write_buffer, sizeof(char), write_size, ob_fp);
    free(write_buffer);
    /* If write failed */
    if (written_size < write_size)
        return ERR_FAILED_TO_WRITE_OB;

    return OK;
}
