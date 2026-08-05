/**
 * assembler.c
 * Handles the control flow between the different assembler stages (pre-assembler, first pass, second pass)
 * while outputting info messages to the user.
 */

#include "assembler.h"

#include "pre_assembler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "globals.h"

#include <stdio.h>


int main(int argc, char *argv[]) {
    return_val rv;

    int file_count;
    char *curr_file_name;

    /* Passed macro table and symbol table */
    macro_table passed_macro_table = {0};
    symbol_table passed_symbol_table = {0};

    /* Code image and data image are stored on stack and passed between stage functions */
    code_entry code_image[MEMORY_SIZE];
    data_entry data_image[MEMORY_SIZE];
    unsigned short ICF, DCF;    /* Final Instruction Counter and Final Data Counter */

    for (file_count = 1; file_count < argc; file_count++) {
        /* Set current file name */
        curr_file_name = argv[file_count];

        /* Separation line */
        printf("\n");

        /* Pre-assembler stage */
        /* Macro table is initialized (and on fail, cleared), by pre-assembler */
        info_message(PRE_ASSEMBLER_START, curr_file_name);
        rv = pre_assembler_loop(curr_file_name, &passed_macro_table);
        info_message(rv, curr_file_name);

        /* If pre-assembler stage failed */
        if (rv == PRE_ASSEMBLER_FAIL)
            continue;

        /* First pass stage */
        /* Symbol table is initialized (and on fail, cleared with macro table), by first pass */
        info_message(FIRST_PASS_START, curr_file_name);
        rv = first_pass_loop(curr_file_name, &passed_macro_table, &passed_symbol_table, code_image, data_image, &ICF, &DCF);
        info_message(rv, curr_file_name);

        /* If first pass stage failed */
        if (rv == FIRST_PASS_FAIL)
            continue;

        /* Second pass stage */
        /* Symbol table and macro table are cleared by second pass */
        info_message(SECOND_PASS_START, curr_file_name);
        info_message(rv, curr_file_name);

        /* If second pass stage failed */
        if (rv == SECOND_PASS_FAIL)
            continue;

        /* Assemble next file */
        continue;
    }

    printf("[Assembler finished]\n");

    return 0;
}
