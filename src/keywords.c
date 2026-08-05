/**
 * keywords.c
 * Responsible for validation checks of reserved keywords.
 * Depends on the validation checks of instructions.c .
 */

#include "keywords.h"

#include "instructions.h"
#include "directives.h"

#include <string.h>


int is_register_keyword(const char *input_string) {
    const char *const registers_talbe[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    const size_t registers_count = sizeof(registers_talbe) / sizeof(registers_talbe[0]);
    size_t i;

    if (!input_string)
        return 0;

    for (i = 0; i < registers_count; i++)
        if (strcmp(registers_talbe[i], input_string) == 0)
            return 1;

    return 0;
}


int is_reserved_keyword(const char *input_string) {
    return is_instruction_keyword(input_string) || is_directive_keyword(input_string) ||
           is_register_keyword(input_string);
}
