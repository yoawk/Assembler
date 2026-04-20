/**
 * instructions.c
 * Responsible for checks and retrievals of the assembly instructions
 */

#include "instructions.h"

#include <string.h>

const instruction_entry *get_instruction(const char *inst_name) {
    static const instruction_entry instructions_table[] = {
        {"mov", 0, 0, 2, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE, DIRECT_MODE | REGISTER_MODE},
        {"cmp", 1, 0, 2, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE},
        {"add", 2, 10, 2, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE, DIRECT_MODE | REGISTER_MODE},
        {"sub", 2, 11, 2, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE, DIRECT_MODE | REGISTER_MODE},
        {"lea", 4, 0, 2, DIRECT_MODE, DIRECT_MODE | REGISTER_MODE},
        {"clr", 5, 10, 1, 0, DIRECT_MODE | REGISTER_MODE},
        {"not", 5, 11, 1, 0, DIRECT_MODE | REGISTER_MODE},
        {"inc", 5, 12, 1, 0, DIRECT_MODE | REGISTER_MODE},
        {"dec", 5, 13, 1, 0, DIRECT_MODE | REGISTER_MODE},
        {"jmp", 9, 10, 1, 0, DIRECT_MODE | RELATIVE_MODE},
        {"bne", 9, 11, 1, 0, DIRECT_MODE | RELATIVE_MODE},
        {"jsr", 9, 12, 1, 0, DIRECT_MODE | RELATIVE_MODE},
        {"red", 12, 0, 1, 0, DIRECT_MODE | REGISTER_MODE},
        {"prn", 13, 0, 1, 0, IMMEDIATE_MODE | DIRECT_MODE | REGISTER_MODE},
        {"rts", 14, 0, 0, 0, 0},
        {"stop", 15, 0, 0, 0, 0}
    };

    static const size_t instructions_count = sizeof(instructions_table) / sizeof(instructions_table[0]);
    size_t i;

    if (!inst_name)
        return NULL;

    /* Go over the instructions and check for a matching instruction name */
    for (i = 0; i < instructions_count; i++)
        if (strcmp(instructions_table[i].name, inst_name) == 0)
            return &(instructions_table[i]);

    /* If inst_name didn't match any instruction name */
    return NULL;

}

int is_instruction_keyword(const char *input_string) {
    if (!input_string)
        return 0;

    if (get_instruction(input_string) != NULL)
        return 1;
    return 0;
}
