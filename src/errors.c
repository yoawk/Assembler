/**
 * errors.c
 * Handles printing of error messages
 */

#include "errors.h"

#include <stdio.h>

#define ERROR_PREFIX_STR "[ ERROR ]"

/*TODO: implement */
void print_error(return_val error, const char *context, const char *file_name, unsigned int line_number) {
    printf("%s ", ERROR_PREFIX_STR);
    switch (error) {


        default:
            printf("Unknown error\n");
    }
}